/* Copyright (C) 2006-2018 PUC-Rio/Laboratorio TeleMidia

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
along with Ginga.  If not, see <https://www.gnu.org/licenses/>.  */

#include "aux-ginga.h"
#include "Document.h"
#include "LuaAPI.h"

#include "Context.h"
#include "Media.h"
#include "Object.h"
#include "Switch.h"

GINGA_NAMESPACE_BEGIN

Document::Document (lua_State *L)
{
  g_return_if_fail (L != NULL);

  _L = L;
  LuaAPI::Document_attachWrapper (_L, this);
}

Document::~Document ()
{
  set<Object *> objects;
  set<StateMachine *> machines;

  this->getStateMachines (&machines);
  for (auto sm: machines)
    delete sm;

  this->getObjects (&objects);
  for (auto obj: objects)
    delete obj;

  LuaAPI::Document_detachWrapper (_L, this);
}

lua_State *
Document::getLuaState ()
{
  return _L;
}

void
Document::getObjects (set<Object *> *objects)
{
  lua_Integer len;
  lua_Integer i;

  g_return_if_fail (objects != NULL);

  LuaAPI::Document_call (_L, this, "getObjects", 0, 1);
  g_assert (lua_type (_L, -1) == LUA_TTABLE);

  len = luaL_len (_L, -1);
  for (i = 1; i <= len; i++)
    {
      Object *obj;

      lua_rawgeti (_L, -1, i);
      obj = LuaAPI::Object_check (_L, -1);
      objects->insert (obj);
      lua_pop (_L, 1);
    }
}

Object *
Document::getObject (const string &id)
{
  Object *obj = NULL;

  lua_pushstring (_L, id.c_str ());
  LuaAPI::Document_call (_L, this, "getObject", 1, 1);
  if (!lua_isnil (_L, -1))
    {
      obj = LuaAPI::Object_check (_L, -1);
    }
  lua_pop (_L, 1);

  return obj;
}

Context *
Document::getRoot ()
{
  Context *root;

  LuaAPI::Document_call (_L, this, "getRoot", 0, 1);
  root = LuaAPI::Context_check (_L, -1);
  lua_pop (_L, 1);

  return root;
}

Media *
Document::getSettings ()
{
  Media *media;

  LuaAPI::Document_call (_L, this, "getSettings", 0, 1);
  media = LuaAPI::Media_check (_L, -1);
  lua_pop (_L, 1);

  return media;
}

Object *
Document::createObject (Object::Type type, const string &id)
{
  Object *obj = NULL;

  LuaAPI::Object_Type_push (_L, type);
  lua_pushstring (_L, id.c_str ());
  LuaAPI::Document_call (_L, this, "createObject", 2, 1);
  if (!lua_isnil (_L, -1))
    {
      obj = LuaAPI::Object_check (_L, -1);
    }
  lua_pop (_L, 1);

  return obj;
}

StateMachine *
Document::getStateMachine (const string &id)
{
  StateMachine *sm = NULL;

  lua_pushstring (_L, id.c_str ());
  LuaAPI::Document_call (_L, this, "getStateMachine", 1, 1);
  if (!lua_isnil (_L, -1))
    {
      sm = LuaAPI::StateMachine_check (_L, -1);
    }
  lua_pop (_L, 1);

  return sm;
}

void
Document::getStateMachines (set<StateMachine *> *machines)
{
  lua_Integer len;
  lua_Integer i;

  g_return_if_fail (machines != NULL);

  LuaAPI::Document_call (_L, this, "getStateMachines", 0, 1);
  g_assert (lua_type (_L, -1) == LUA_TTABLE);

  len = luaL_len (_L, -1);
  for (i = 1; i <= len; i++)
    {
      StateMachine *sm;

      lua_rawgeti (_L, -1, i);
      sm = LuaAPI::StateMachine_check (_L, -1);
      machines->insert (sm);
      lua_pop (_L, 1);
    }
}

StateMachine *
Document::createStateMachine (StateMachine::Type type, Object *obj,
                              const string &smId)
{
  StateMachine *sm = NULL;

  g_return_val_if_fail (obj != NULL, NULL);

  LuaAPI::StateMachine_Type_push (_L, type);
  LuaAPI::Object_push (_L, obj);
  lua_pushstring (_L, smId.c_str ());
  LuaAPI::Document_call (_L, this, "createStateMachine", 3, 1);
  if (!lua_isnil (_L, -1))
    {
      sm = LuaAPI::StateMachine_check (_L, -1);
    }
  lua_pop (_L, 1);

  return sm;
}

StateMachine *
Document::createStateMachine (StateMachine::Type type, const string &objId,
                              const string &smId)
{
  StateMachine *sm = NULL;

  LuaAPI::StateMachine_Type_push (_L, type);
  lua_pushstring (_L, objId.c_str ());
  lua_pushstring (_L, smId.c_str ());
  LuaAPI::Document_call (_L, this, "createStateMachine", 3, 1);
  if (!lua_isnil (_L, -1))
    {
      sm = LuaAPI::StateMachine_check (_L, -1);
    }
  lua_pop (_L, 1);

  return sm;
}

StateMachine *
Document::createStateMachine (const string &qualId)
{
  StateMachine *sm = NULL;

  lua_pushstring (_L, qualId.c_str ());
  LuaAPI::Document_call (_L, this, "createStateMachine", 1, 1);
  if (!lua_isnil (_L, -1))
    {
      sm = LuaAPI::StateMachine_check (_L, -1);
    }
  lua_pop (_L, 1);

  return sm;
}

lua_Integer
Document::getTime ()
{
  lua_Integer time;

  LuaAPI::Document_call (_L, this, "getTime", 0, 1);
  time = luaL_checkinteger (_L, -1);
  lua_pop (_L, 1);

  return time;
}

void
Document::advanceTime (lua_Integer dt)
{
  lua_pushinteger (_L, dt);
  LuaAPI::Document_call (_L, this, "advanceTime", 1, 0);
}

void
Document::sendKey (const string &key, bool press)
{
  lua_pushstring (_L, key.c_str ());
  lua_pushboolean (_L, press);
  LuaAPI::Document_call (_L, this, "sendKey", 2, 0);
}

void
Document::draw (cairo_t *cr)
{
  lua_Integer width;
  lua_Integer height;

  lua_Integer len;
  lua_Integer i;

  g_assert (this->getSettings ()->getPropertyInteger ("width", &width));
  g_assert (this->getSettings ()->getPropertyInteger ("height", &height));

  // Clear frame.
  cairo_save (cr);
  cairo_set_source_rgba (cr, 0, 0, 0, 1.0);
  cairo_rectangle (cr, 0, 0, (double) width, (double) height);
  cairo_fill (cr);
  cairo_restore (cr);

  // Draw background color.
  cairo_save (cr);
  cairo_set_source_rgba (cr, .5, 0, .5, 1);
  cairo_rectangle (cr, 0, 0, (double) width, (double) height);
  cairo_fill (cr);
  cairo_restore (cr);

  // Draw players.
  LuaAPI::Document_call (_L, this, "_getPlayers", 0, 1);
  g_assert (lua_type (_L, -1) == LUA_TTABLE);
  len = luaL_len (_L, -1);
  for (i = 1; i <= len; i++)
    {
      Player *player;

      lua_rawgeti (_L, -1, i);
      player = LuaAPI::Player_check (_L, -1);
      player->draw (cr);
      lua_pop (_L, 1);
    }
  lua_pop (_L, 1);
}

// TODO --------------------------------------------------------------------

int
Document::evalAction (StateMachine *sm,
                      StateMachine::Transition transition,
                      const string &value)
{
  Action act;
  act.target = sm->getQualifiedId ();
  g_assert_nonnull (sm);
  act.transition = transition;
  act.predicate = NULL;
  act.params["value"] = value;
  return this->evalAction (act);
}


list<Action>
Document::evalActionInContext (Action act, Context *ctx)
{
  list<Action> stack;
  StateMachine *sm;

  sm = this->getStateMachine (act.target);
  g_assert_nonnull (sm);

  // if (!ctx->getLinksStatus ())
  //   return stack;
  for (auto link : *ctx->getLinks ())
    {
      for (auto cond : link.first)
        {
          Predicate *pred;

          if (cond.target != sm->getQualifiedId ()
              || cond.transition != act.transition)
            continue;

          pred = cond.predicate;
          if (pred != NULL && !this->evalPredicate (pred))
            continue;

          // Success.
          auto acts = link.second;
          for (auto ri = acts.rbegin (); ri != acts.rend (); ++ri)
            {
              Action next_act = *(ri);
              string s;
              lua_Integer delay;

              if (!this->evalPropertyRef (next_act.params["delay"], &s))
                {
                  s = next_act.params["delay"];
                }

              //delay = ginga::parse_time (s);
              delay = 0;

              if (delay == 0 || delay == -1)
                {
                  stack.push_back (*ri);
                }
              else
                {
                  StateMachine *next_sm;

                  next_sm = this->getStateMachine (next_act.target);
                  g_assert_nonnull (next_sm);

                  Object *next_obj = next_sm->getObject ();
                  g_assert_nonnull (next_obj);
                }
            }
        }
    }
  return stack;
}

int
Document::evalAction (Action init)
{
  list<Action> stack;
  int n;

  stack.push_back (init);
  n = 0;

  while (!stack.empty ())
    {
      Action act;
      StateMachine *sm;
      Object *obj;
      Composition *comp;
      Context *ctx_parent, *ctx_grandparent;
      map<string, string> params;

      act = stack.back ();
      stack.pop_back ();

      sm = this->getStateMachine (act.target);
      g_assert_nonnull (sm);

      params["duration"] = act.params["duration"];
      if (sm->getType () == StateMachine::ATTRIBUTION)
        {
          params["value"] = act.params["value"];
        }

      if (!sm->transition (act.transition, params))
        continue;

      n++;
      obj = sm->getObject ();
      g_assert_nonnull (obj);

      set<Composition *> parents;
      obj->getParents (&parents);
      auto it = parents.begin ();
      comp = (it == parents.end ()) ? NULL: *it;

      // If parent composition is a context
      if (comp != NULL &&
          instanceof (Context *, comp)
          && comp->getLambda ()->getState () == StateMachine::OCCURRING)
        {
          ctx_parent = cast (Context *, comp);
          g_assert_nonnull (ctx_parent);

          // Trigger links in the parent context
          list<Action> ret = evalActionInContext (act, ctx_parent);
          stack.insert (stack.end (), ret.begin (), ret.end ());

          set<Composition *> parents;
          ctx_parent->getParents (&parents);
          auto it = parents.begin ();
          Composition *comp = (it == parents.end ()) ? NULL: *it;

          if (comp != NULL &&
              instanceof (Context *, comp)
              && comp->getLambda ()->getState () == StateMachine::OCCURRING)
            {
              ctx_grandparent = cast (Context *, comp);
              list<StateMachine *> ports;
              ctx_parent->getPorts (&ports);
              for (auto port : ports)
                {
                  if (port->getObject () == sm->getObject ())
                    {
                      list<Action> ret
                          = evalActionInContext (act, ctx_grandparent);
                      stack.insert (stack.end (), ret.begin (), ret.end ());
                    }
                }
            }
        }
      // If parent composition is a switch
      else if (comp != NULL && instanceof (Switch *, comp))
        {
          // Trigger the switchPort labelled action mapped to the switch's
          // media object.
          Switch *swtch = cast (Switch *, comp);
          for (const auto &swtchPort : *swtch->getSwitchPorts ())
            {
              for (const auto &mapped_sm : swtchPort.second)
                {
                  set<Composition *> parents;
                  swtch->getParents (&parents);
                  auto it = parents.begin ();
                  Composition *sparent = (it == parents.end ()) ? NULL: *it;

                  if (mapped_sm->getObject () == sm->getObject ()
                      && sparent != NULL)
                    {
                      StateMachine *label_sm = swtch->getStateMachine (
                          StateMachine::PRESENTATION, swtchPort.first);
                      g_assert_nonnull (label_sm);

                      // Do the same action in the "equivalent" switchPort
                      Action label_act = act;
                      label_act.target = label_sm->getQualifiedId ();
                      evalAction (label_act);
                    }
                }
            }
        }

      // If state machine is a context, trigger the context itself
      if (instanceof (Context *, obj))
        {
          ctx_parent = cast (Context *, obj);
          g_assert_nonnull (ctx_parent);
          list<Action> ret = evalActionInContext (act, ctx_parent);
          stack.insert (stack.end (), ret.begin (), ret.end ());
        }
      // If have refer elements, trigger in the contexts
      // else if (obj->getAliases ()->size ())
      //   {
      //     for (const auto &alias : *obj->getAliases ())
      //       {
      //         ctx_parent = cast (Context *, alias.second);
      //         if (ctx_parent)
      //           {
      //             list<Action> ret = evalActionInContext (act, ctx_parent);
      //             stack.insert (stack.end (), ret.begin (), ret.end ());
      //           }
      //       }
      //   }
    }
  return n;
}

bool
Document::evalPredicate (Predicate *pred)
{
  switch (pred->getType ())
    {
    case Predicate::FALSUM:
      TRACE ("false");
      return false;
      break;
    case Predicate::VERUM:
      TRACE ("true");
      return true;
      break;
    case Predicate::ATOM:
      {
        string left, right;
        Predicate::Test test;
        string msg_left, msg_test, msg_right;
        bool result;

        pred->getTest (&left, &test, &right);

        if (left[0] == '$')
          {
            msg_left = left;
            if (this->evalPropertyRef (left, &left))
              msg_left += " ('" + left + "')";
          }
        else
          {
            msg_left = "'" + left + "'";
          }

        if (right[0] == '$')
          {
            msg_right = right;
            if (this->evalPropertyRef (right, &right))
              msg_right += " ('" + right + "')";
          }
        else
          {
            msg_right = "'" + right + "'";
          }

        switch (test)
          {
          case Predicate::EQ:
            msg_test = "==";
            result = left == right;
            break;
          case Predicate::NE:
            msg_test = "!=";
            result = left != right;
            break;
          case Predicate::LT:
            msg_test = "<";
            result = left < right;
            break;
          case Predicate::LE:
            msg_test = "<=";
            result = left <= right;
            break;
          case Predicate::GT:
            msg_test = ">";
            result = left > right;
            break;
          case Predicate::GE:
            msg_test = ">=";
            result = left >= right;
            break;
          default:
            g_assert_not_reached ();
          }
        TRACE ("%s %s %s -> %s", msg_left.c_str (), msg_test.c_str (),
               msg_right.c_str (), strbool (result));
        return result;
      }
    case Predicate::NEGATION:
      g_assert_not_reached ();
      break;
    case Predicate::CONJUNCTION:
      {
        for (auto child : *pred->getChildren ())
          {
            if (!this->evalPredicate (child))
              {
                TRACE ("and -> false");
                return false;
              }
          }
        TRACE ("and -> true");
        return true;
      }
      break;
    case Predicate::DISJUNCTION:
      {
        for (auto child : *pred->getChildren ())
          {
            if (this->evalPredicate (child))
              {
                TRACE ("or -> true");
                return true;
              }
          }
        TRACE ("or -> false");
        return false;
      }
      break;
    default:
      g_assert_not_reached ();
    }
  g_assert_not_reached ();
}

bool
Document::evalPropertyRef (const string &ref, string *result)
{
  size_t i;
  string id;
  string name;
  Object *object;

  g_return_val_if_fail (result != NULL, false);

  if (ref[0] != '$' || (i = ref.find ('.')) == string::npos)
    return false;

  id = ref.substr (1, i - 1);
  name = ref.substr (i + 1);
  object = this->getObject (id);
  if (object == NULL)
    {
      return false;
    }

  return object->getPropertyString (name, result);
}

bool
Document::getData (const string &key, void **value)
{
  return _udata.getData (key, value);
}

bool
Document::setData (const string &key, void *value, UserDataCleanFunc fn)
{
  return _udata.setData (key, value, fn);
}

GINGA_NAMESPACE_END