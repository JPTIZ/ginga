/* Copyright (C) 2006-2017 PUC-Rio/Laboratorio TeleMidia

This file is part of Ginga (Ginga-NCL).

Ginga is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

Ginga is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License
along with Ginga.  If not, see <http://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "Converter.h"

#include "NclAction.h"
#include "Scheduler.h"

GINGA_FORMATTER_BEGIN

Converter::Converter (GingaInternal *ginga,
                      INclActionListener *actlist,
                      RuleAdapter *ruleAdapter)
{
  g_assert_nonnull (ginga);
  _ginga = ginga;

  _scheduler = ginga->getScheduler ();
  g_assert_nonnull (_scheduler);

  _actionListener = actlist;
  _ruleAdapter = ruleAdapter;
}

Converter::~Converter ()
{
}

RuleAdapter *
Converter::getRuleAdapter ()
{
  return _ruleAdapter;
}

NclEvent *
Converter::getEvent (ExecutionObject *exeObj,
                     Anchor *interfacePoint,
                     EventType ncmEventType,
                     const string &key)
{
  string id;
  string suffix;

  NclEvent *event;
  string type;

  switch (ncmEventType)
    {
    case EventType::SELECTION:
      suffix = "<sel";
      if (key != "")
        suffix += "(" + key + ")";
      suffix += ">";
      break;
    case EventType::PRESENTATION:
      suffix = "<pres>";
      break;
    case EventType::ATTRIBUTION:
      suffix = "<attr>";
      break;
    default:
      g_assert_not_reached ();
    }

  id = interfacePoint->getId () + suffix;

  event = exeObj->getEvent (id);
  if (event != nullptr)
    {
      return event;
    }

  auto switchObj = cast (ExecutionObjectSwitch *, exeObj);
  auto cObj = cast (ExecutionObjectContext *, exeObj);

  if (switchObj)
    {
      event = new SwitchEvent (_ginga,
            id, switchObj, interfacePoint, ncmEventType, key);
    }
  else if (ncmEventType == EventType::PRESENTATION)
    {
      event = new PresentationEvent (_ginga,
            id, exeObj, (Area *)interfacePoint);
    }
  else if (cObj)
    {
      if (ncmEventType == EventType::ATTRIBUTION)
        {
          auto propAnchor = cast (Property *, interfacePoint);
          g_assert_nonnull (propAnchor);
          event = new AttributionEvent (_ginga, id, exeObj, propAnchor);
        }
    }
  else
    {
      switch (ncmEventType)
        {
        case EventType::ATTRIBUTION:
          {
            auto propAnchor = cast (Property *, interfacePoint);
            g_assert_nonnull (propAnchor);
            event = new AttributionEvent (_ginga, id, exeObj, propAnchor);

            break;
          }
        case EventType::SELECTION:
          {
            event = new SelectionEvent (_ginga,
                                        id, exeObj, (Area *)interfacePoint);
            if (key != "")
              {
                ((SelectionEvent *)event)->setSelectionCode (key);
              }
            break;
          }
        default:
          g_assert_not_reached ();
        }
    }

  g_assert_nonnull (event);
  exeObj->addEvent (event);

  return event;
}

void
Converter::processLink (Link *ncmLink,
                        Node *dataObject,
                        ExecutionObject *executionObject,
                        ExecutionObjectContext *parentObject)
{
  Node *nodeEntity = nullptr;
  const set<Refer *> *sameInstances;
  bool contains = false;

  if (executionObject->getNode () != nullptr)
    nodeEntity = cast (Node *, executionObject->getNode ());

  if (!parentObject->containsUncompiledLink (ncmLink))
    return;

  auto causalLink = cast (Link *, ncmLink);
  g_assert_nonnull (causalLink);

  if (nodeEntity != nullptr && instanceof (Media *, nodeEntity))
    {
      sameInstances = cast (Media *, nodeEntity)
        ->getInstSameInstances ();
      for (Refer *referNode: *sameInstances)
        {
          contains = causalLink->contains (referNode, true);
          if (contains)
            break;
        }
    }

  // Checks if execution object is part of link conditions.
  if (causalLink->contains (dataObject, true) || contains)
    {
      parentObject->removeLinkUncompiled (ncmLink);
      NclLink *formatterLink
        = createLink (causalLink, parentObject);

      if (formatterLink != NULL)
        parentObject->setLinkCompiled (formatterLink);
    }
}

void
Converter::compileExecutionObjectLinks (
    ExecutionObject *exeObj, Node *dataObject,
    ExecutionObjectContext *parentObj)
{
  set<Link *> *uncompiledLinks;
  ExecutionObjectContext *compObj;
  Node *execDataObject;

  exeObj->setCompiled (true);

  if (parentObj == nullptr)
    return;

  execDataObject = exeObj->getNode ();
  if (execDataObject != dataObject)
    {
      compObj = parentObj->getParent ();
      if (compObj != nullptr && compObj != parentObj)
        {
          compileExecutionObjectLinks (exeObj, execDataObject,
                                       compObj);
        }
    }

  uncompiledLinks = parentObj->getUncompiledLinks ();
  if (!uncompiledLinks->empty ())
    {
      set<Link *> *dataLinks = uncompiledLinks;

      for ( Link *ncmLink : *dataLinks)
        {
          processLink (ncmLink, dataObject, exeObj, parentObj);
        }

      delete dataLinks;

      compileExecutionObjectLinks (
            exeObj, dataObject,
            parentObj->getParent ());
    }
  else
    {
      ExecutionObject *object;

      delete uncompiledLinks;

      while (parentObj != nullptr)
        {
          object = parentObj;
          parentObj = parentObj->getParent ();
          compileExecutionObjectLinks (object, dataObject, parentObj);
        }
    }
}

ExecutionObject *
Converter::processExecutionObjectSwitch (
    ExecutionObjectSwitch *switchObject)
{

  Node *selectedNode;
  string id;
  map<string, ExecutionObject *>::iterator i;
  ExecutionObject *selectedObject;

  auto switchNode = cast (Switch *, switchObject->getNode ());
  g_assert_nonnull (switchNode);

  selectedNode = _ruleAdapter->adaptSwitch (switchNode);
  g_assert_nonnull (selectedNode);

  ExecutionObject *obj = _scheduler->getObjectByIdOrAlias (selectedNode->getId ());
  if (obj != nullptr)
    {
      switchObject->select (obj);
      resolveSwitchEvents (switchObject);
      return obj;
    }

  selectedObject = obtainExecutionObject (selectedNode);
  g_assert_nonnull (selectedNode);

  switchObject->select (selectedObject);
  resolveSwitchEvents (switchObject);

  return selectedObject;
}

void
Converter::resolveSwitchEvents (
    ExecutionObjectSwitch *switchObject)
{
  ExecutionObject *selectedObject;
  ExecutionObject *endPointObject;
  Node *selectedNode;
  vector<NclEvent *> events;
  vector<NclEvent *>::iterator i;
  SwitchEvent *switchEvent;
  Anchor *interfacePoint;
  vector<Node *> nestedSeq;
  NclEvent *mappedEvent;

  selectedObject = switchObject->getSelectedObject ();
  g_assert_nonnull (selectedObject);

  selectedNode = selectedObject->getNode ();
  selectedNode = cast (Node *, selectedNode);
  g_assert_nonnull (selectedNode);

  for (NclEvent *event: switchObject->getEvents ())
    {
      mappedEvent = nullptr;
      switchEvent = cast (SwitchEvent *, event);
      g_assert_nonnull (switchEvent);

      interfacePoint = switchEvent->getInterface ();
      auto lambdaAnchor = cast (AreaLambda *, interfacePoint);
      if (lambdaAnchor)
        {
          mappedEvent = getEvent (
                selectedObject, selectedNode->getLambda (),
                switchEvent->getType (), switchEvent->getKey ());
        }
      else
        {
          auto switchPort = cast (SwitchPort *, interfacePoint);
          g_assert_nonnull (switchPort);

          for (Port *mapping: *(switchPort->getPorts ()))
            {
              if (mapping->getNode () == selectedNode)
                {
                  endPointObject = obtainExecutionObject (selectedNode);

                  if (endPointObject != nullptr)
                    {
                      mappedEvent = getEvent (
                            endPointObject,
                            mapping->getFinalInterface (),
                            switchEvent->getType (),
                            switchEvent->getKey ());
                    }
                  break;
                }
            }
        }

      if (mappedEvent != nullptr)
        {
          switchEvent->setMappedEvent (mappedEvent);
        }
    }
}

NclEvent *
Converter::insertContext (Port *port)
{
  Anchor *anchor;
  vector<Node *> nestedSeq;
  EventType eventType;

  g_assert_nonnull (port);

  anchor = port->getFinalInterface ();

  g_assert (instanceof (Area *, anchor)
            || instanceof (AreaLabeled *, anchor)
            || instanceof (Property *, anchor)
            || instanceof (SwitchPort *, anchor));

  nestedSeq = port->getMapNodeNesting ();

  ExecutionObject *object
    = obtainExecutionObject (nestedSeq.back ());
  g_assert_nonnull (object);
  if (instanceof (Property *, anchor))
    {
      eventType = EventType::ATTRIBUTION;
    }
  else
    {
      eventType = EventType::PRESENTATION;
    }

  return getEvent (object, anchor, eventType, "");
}

void
Converter::eventStateChanged (NclEvent *event,
                              EventStateTransition transition,
                              unused (EventState previousState))
{
  ExecutionObject *exeObj = event->getExecutionObject ();
  auto exeSwitch = cast (ExecutionObjectSwitch *, exeObj);

  if (exeSwitch)
    {
      if (transition == EventStateTransition::START)
        {
          for (NclEvent *e: exeSwitch->getEvents())
            {
              auto switchEvt = cast (SwitchEvent *, e);
              if (switchEvt)
                {
                  NclEvent *ev = switchEvt->getMappedEvent ();

                  if (ev == nullptr)
                    {
                      // there is only one way to start a switch with
                      // NULL mapped event: a instSame refernode inside
                      // it was started
                      processExecutionObjectSwitch (exeSwitch);

                      ev = switchEvt->getMappedEvent ();
                      if (ev != nullptr)
                        {
                          // now we know the event is mapped, we can start
                          // the
                          // switchport
                          e->start ();
                        }
                    }
                }
            }
        }

      if (transition == EventStateTransition::STOP
          || transition == EventStateTransition::ABORT)
        {
          exeSwitch->select (NULL);
        }
    }
}

NclEvent *
Converter::createEvent (Bind *bind)
{
  ExecutionObject *executionObject;
  Anchor *interfacePoint;
  string key;
  NclEvent *event = nullptr;
  vector<Node *> seq;

  Node *node = bind->getNode ();
  g_assert_nonnull (node);

  interfacePoint = bind->getInterface ();

  seq.push_back (node);
  if (interfacePoint != nullptr
      && instanceof (Port *, interfacePoint)
      && !(instanceof (SwitchPort *, interfacePoint)))
    {
      for (auto inner: ((Port *) interfacePoint)->getMapNodeNesting ())
        seq.push_back (inner);
    }


  executionObject = obtainExecutionObject (seq.back());
  g_assert_nonnull (executionObject);

  if (interfacePoint == nullptr)
    {
      return executionObject->getWholeContentPresentationEvent ();
    }

  if (instanceof (Composition *, node)
      && instanceof (Port *, interfacePoint))
    {
      Composition *comp = cast (Composition *, node);
      Port *port = cast (Port *, interfacePoint);
      interfacePoint = comp->getMapInterface (port);
    }

  if (!getBindKey (bind, &key))
    key = "";
  event = getEvent (executionObject, interfacePoint,
                    bind->getRole ()->getEventType (), key);

  return event;
}

bool
Converter::getBindKey (Bind *bind, string *result)
{
  Role *role;
  Condition *docCond;
  string key;

  role = bind->getRole ();
  g_assert_nonnull (role);

  docCond = cast (Condition *, role);
  if (docCond == nullptr)
    return false;

  key = docCond->getKey ();
  if (key[0] == '$')
    key = bind->getParameter (key.substr (1, key.length () - 1));

  tryset (result, key);
  return true;
}


// INSANITY ABOVE ----------------------------------------------------------

ExecutionObject *
Converter::obtainExecutionObject (Node *node)
{
  string id;
  Node *parentNode;
  ExecutionObjectContext *parent;
  ExecutionObject *object;
  PresentationEvent *event;

  id = node->getId ();
  g_assert (id != "");

  // Already created.
  if ((object = _scheduler->getObjectByIdOrAlias (id)) != nullptr)
    return object;

  // Get parent.
  parentNode = node->getParent ();
  if (parentNode == nullptr)
    {
      parent = nullptr;
    }
  else
    {
      parent = cast (ExecutionObjectContext *,
                     obtainExecutionObject (parentNode));
      g_assert_nonnull (parent);
    }

  if (instanceof (Refer *, node))
    {
      Node *target;

      TRACE ("solving refer");
      target = node->derefer ();
      g_assert (!instanceof (Refer *, target));
      object = obtainExecutionObject (target->derefer ());
      object->addAlias (id);
      return object;
    }
  else if (instanceof (Context *, node))
    {
      TRACE ("creating switch");
      object = new ExecutionObjectContext
        (_ginga, id, node, _actionListener);
      event = new PresentationEvent
        (_ginga, node->getLambda ()->getId () + "<pres>", object,
         (Area *)(node->getLambda ()));
      object->addEvent (event);
    }
  else if (instanceof (Switch *, node))
    {
      TRACE ("creating switch");
      object = new ExecutionObjectSwitch
        (_ginga, id, node, _actionListener);
      event = new PresentationEvent
        (_ginga, node->getLambda ()->getId () + "<pres>", object,
         (Area *)(node->getLambda ()));
      object->addEvent (event);
    }
  else if (instanceof (Media *, node))
    {
      Media *media;
      media = cast (Media *, node);
      g_assert_nonnull (media);
      if (media->isSettings ())
        {
          object = new ExecutionObjectSettings
            (_ginga, id, node, _actionListener);
          _ruleAdapter->setSettings (object);
        }
      else
        {
          object = new ExecutionObject
            (_ginga, id, node, _actionListener);
          compileExecutionObjectLinks
            (object, node, cast (ExecutionObjectContext *, parent));
        }
    }
  else
    {
      g_assert_not_reached ();
    }

  g_assert_nonnull (object);
  if (parent != nullptr)
    object->initParent (parent);
  _scheduler->addObject (object);
  return object;
}

NclLink *
Converter::createLink (Link *docLink, ExecutionObjectContext *context)
{
  Connector *connector;
  NclLink *link;

  g_assert_nonnull (docLink);
  g_assert_nonnull (context);

  connector = cast (Connector *, docLink->getConnector ());
  g_assert_nonnull (connector);

  link = new NclLink (context);

  // Add conditions.
  for (auto connCond: *connector->getConditions ())
    {
      for (auto bind: docLink->getBinds (connCond))
        {
          NclCondition *cond;
          cond = this->createCondition (connCond, bind);
          g_assert_nonnull (cond);
          g_assert (link->addCondition (cond));
        }
    }

  // Add actions.
  for (auto connAct: *connector->getActions ())
    {
      for (auto bind: docLink->getBinds (connAct))
        {
          NclAction *action;
          action = this->createAction (connAct, bind);
          g_assert_nonnull (action);
          g_assert (link->addAction (action));
        }
    }

  return link;
}

NclCondition *
Converter::createCondition (Condition *connCondition, Bind *bind)
{
  NclEvent *event;

  g_assert_nonnull (connCondition);
  g_assert_nonnull (bind);

  event = createEvent (bind);
  g_assert_nonnull (event);
  return new NclCondition (event, connCondition->getTransition ());
}

NclAction *
Converter::createAction (Action *connAction, Bind *bind)
{
  EventType eventType;
  EventStateTransition transition;

  NclEvent *event;
  NclAction *action;

  g_assert_nonnull (connAction);
  g_assert_nonnull (bind);

  eventType = bind->getRole ()->getEventType ();
  transition = connAction->getTransition ();

  event = createEvent (bind);
  g_assert_nonnull (event);
  event->setType (eventType);

  action = new NclAction (event, transition, _actionListener);
  if (eventType == EventType::ATTRIBUTION)
    {
      string dur;
      string value;

      dur = connAction->getDuration ();
      if (dur[0] == '$')
          dur = bind->getParameter (dur.substr (1, dur.length () - 1));

      value = connAction->getValue ();
      if (value[0] == '$')
        value = bind->getParameter (value.substr (1, value.length () - 1));

      g_assert (dur[0] != '$');
      g_assert (value[0] != '$');

      action->setDuration (dur);
      action->setValue (value);
    }

  return action;
}


GINGA_FORMATTER_END
