-- Compares players by zIndex and zOrder.
local function comparePlayers (p1, p2)
   local m1, m2 = p1.media, p2.media
   local z1 = tonumber (m1.property.zIndex) or math.mininteger
   local z2 = tonumber (m2.property.zIndex) or math.mininteger
   if z1 < z2 then
      return true
   end
   if z1 > z2 then
      return false
   end
   local zo1 = tonumber (m1.property.zOrder) or math.mininteger
   local zo2 = tonumber (m2.property.zOrder) or math.mininteger
   if zo1 < zo2 then
      return true
   else
      return false
   end
end

-- Dumps object graph rooted at node.
local function dumpGraph (node, prefix)
   local prefix = prefix or ''
   if node:isComposition () then
      print (('%s+ %s::%s'):format (prefix, node.id, node.type))
      local comps = {}
      local atoms = {}
      for _,child in ipairs (node:getChildren ()) do
         if child:isComposition () then
            table.insert (comps, child)
         else
            table.insert (atoms, child)
         end
      end
      for _,atom in ipairs (atoms) do
         dumpGraph (atom, prefix..'  |')
      end
      if #comps == 0 then
         print (prefix)
      else
         for i=1,#comps-1 do
            dumpGraph (comps[i], prefix..'  |')
         end
         dumpGraph (comps[#comps], prefix..'  ')
      end
   else
      print (('%s- %s::%s'):format (prefix, node.id, node.type))
   end
end

-- Parses qualified id.
--
-- Returns the resulting event type, object id, and event id if successful;
-- otherwise returns nil.
--
local function parseQualifiedId (id)
   local tp, o, e

   o, e = id:match ('([%w_-]+)@([%w_-]+)')
   if o and e then
      if e == 'lambda' then
         e = '@lambda'
      end
      tp = 'presentation'
      goto tail
   end

   o, e = id:match ('([%w_-]+)%.([%w._-]+)')
   if o and e then
      tp = 'attribution'
      goto tail
   end

   o, e = id:match ('([%w_-]+)<([%w_-]*)>')
   if o and e then
      tp = 'selection'
   else
      return nil             -- bad format
   end

   ::tail::
   assert (tp)
   assert (o)
   assert (e)
   return tp, o, e
end

-- Document metatable.
do
   local mt = ...

   -- Attaches private data and access functions.
   local saved_attachData = assert (mt._attachData)
   mt._attachData = function (self, data, funcs)
      local data = data or {}
      local funcs = funcs or {}

      -- Private data.
      data._object   = {}       -- objects indexed by id
      data._event    = {}       -- events indexed by qualified id
      data._parents  = {}       -- table of parents indexed by object
      data._children = {}       -- table of children indexed by object
      data._players  = {}       -- list of players sorted by zIndex

      local get_data_object = function ()
         return assert (rawget (data, '_object'))
      end
      local get_data_event = function ()
         return assert (rawget (data, '_event'))
      end
      local get_data_parents = function ()
         return rawget (data, '_parents')
      end
      local get_data_children = function ()
         return rawget (data, '_children')
      end

      -- Getters & setters.
      funcs.objects  = {mt.getObjects,     nil}
      funcs.root     = {mt.getRoot,        nil}
      funcs.settings = {mt.getSettings,    nil}
      funcs.events   = {mt.getEvents,      nil}
      --
      funcs.object   = {get_data_object,   nil}
      funcs.event    = {get_data_event,    nil}
      funcs.parents  = {get_data_parents,  nil}
      funcs.children = {get_data_children, nil}

      return saved_attachData (self, data, funcs)
   end

   -- Initializes private data.
   local saved_init = assert (mt._init)
   mt._init = function (self)
      assert (self:createObject ('context', '__root__'))
      assert (self:createObject ('media', '__settings__'))
      return saved_init (self)
   end

   -- Finalizes private data.
   local saved_fini = assert (mt._fini)
   mt._fini = function (self)
      return saved_fini (self)
   end

   -- Adds object to document.
   mt._addObject = function (self, obj)
      assert (obj.document == self)
      assert (rawget (mt[self], '_object'))[obj.id] = obj
      assert (rawget (mt[self], '_parents'))[obj] = {}
      if obj:isComposition () then
         assert (rawget (mt[self], '_children'))[obj] = {}
      end
   end

   -- Removes object from document.
   mt._removeObject = function (self, obj)
      assert (obj.document == self)
      for _,parent in ipairs (obj.parents) do
         parent:removeChild (obj)
      end
      if obj:isComposition () then
         for _,child in ipairs (obj.children) do
            obj:removeChild (child)
         end
      end
      assert (rawget (mt[self], '_object'))[obj.id] = nil
   end

   -- Adds event to document.
   mt._addEvent = function (self, evt)
      assert (evt.object.document == self)
      assert (rawget (mt[self], '_event'))[evt.qualifiedId] = evt
   end

   -- Removes event from document.
   mt._removeEvent = function (self, evt)
      assert (evt.object.document == self)
      assert (rawget (mt[self], '_event'))[evt.qualifiedId] = nil
   end

   -- Gets the list of parents of obj.
   mt._getParents = function (self, obj)
      assert (obj.document == self)
      local t = {}
      for k,_ in pairs (self.parents[obj] or {}) do
         table.insert (t, k)
      end
      return t
   end

   -- Gets the list of children of obj.
   mt._getChildren = function (self, obj)
      assert (obj.document == self)
      assert (obj:isComposition ())
      local t = {}
      for k,_ in pairs (assert (self.children[obj])) do
         table.insert (t, k)
      end
      return t
   end

   -- Adds object to parent.
   mt._addChild = function (self, parent, obj)
      assert (parent.document == self)
      assert (obj.document == self)
      assert (parent:isComposition ())
      local tchildren = assert (self.children[parent])
      tchildren[obj] = true
      local tparents = assert (self.parents[obj])
      tparents[parent] = true
   end

   -- Removes object from parent.
   mt._removeChild = function (self, parent, obj)
      assert (parent.document == self)
      assert (obj.document == self)
      local tchildren = assert (self.children[parent])
      tchildren[obj] = nil
      local tparents = assert (self.parents[obj])
      tparents[parent] = nil
   end

   -- Adds player to document.
   mt._addPlayer = function (self, player)
      local media = assert (player.media)
      assert (media.document == self)
      local players = assert (rawget (mt[self], '_players'))
      table.insert (players, player)
      self:_sortPlayers ()
   end

   -- Removes player from document.
   mt._removePlayer = function (self, player)
      local media = assert (player.media)
      assert (media.document == self)
      local players = assert (rawget (mt[self], '_players'))
      for i,v in ipairs (players) do
         if player == v then
            table.remove (players, i)
            self:_sortPlayers ()
            break
         end
      end
   end

   -- Gets the list of players in document sorted by zIndex.
   -- Warning: This list is read-only!
   mt._getPlayers = function (self)
      return assert (rawget (mt[self], '_players'))
   end

   -- Sorts the list of players by zIndex.
   mt._sortPlayers = function (self)
      local players = assert (rawget (mt[self], '_players'))
      table.sort (players, comparePlayers)
   end

   -- File extension to mime-type.
   mt._mimetable = {
      ['ac3']    = 'audio/ac3',
      ['avi']    = 'video/x-msvideo',
      ['bmp']    = 'image/bmp',
      ['gif']    = 'image/gif',
      ['jpeg']   = 'image/jpeg',
      ['jpg']    = 'image/jpeg',
      ['lua']    = 'application/x-ginga-NCLua',
      ['mov']    = 'video/quicktime',
      ['mp2']    = 'audio/mp2',
      ['mp3']    = 'audio/mp3',
      ['mp4']    = 'video/mp4',
      ['mpa']    = 'audio/mpa',
      ['mpeg']   = 'video/mpeg',
      ['mpg']    = 'video/mpeg',
      ['mpv']    = 'video/mpv',
      ['oga']    = 'audio/ogg',
      ['ogg']    = 'audio/ogg',
      ['ogv']    = 'video/ogg',
      ['opus']   = 'audio/ogg',
      ['png']    = 'image/png',
      ['spx']    = 'audio/ogg',
      ['srt']    = 'text/srt',
      ['svg']    = 'image/svg+xml',
      ['svgz']   = 'image/svg+xml',
      ['ts']     = 'video/mpeg',
      ['txt']    = 'text/plain',
      ['wav']    = 'audio/basic',
      ['webp']   = 'image/x-webp',
      ['wmv']    = 'video/x-ms-wmv',
      ['xml']    = 'text/xml',
   }

   -- Gets player name from mime-type and URI.
   mt._getPlayerName = function (self, type, uri)
      local str
      if type == nil then       -- use uri to determine type
         if uri == nil then
            goto default
         end
         local ext = uri:match ('.*%.(.*)$')
         if ext == nil then
            goto default
         end
         type = assert (mt._mimetable)[ext]
      end
      if type == nil then
         goto default
      end
      if type == 'application/x-ginga-NCLua' then
         return type, 'PlayerLua'
      end
      if type == 'image/svg+xml' then
         return type, 'PlayerSvg'
      end
      str = type:match ('^([^/]*)/?')
      if str == nil then
         goto default
      end
      if str == 'audio' or str == 'video' then
         return type, 'PlayerVideo'
      end
      if str == 'image' then
         return type, 'PlayerImage'
      end
      if str == 'text' then
         return type, 'PlayerText'
      end
      ::default::
      return 'application/x-ginga-timer', nil
   end

   -- Dumps document graph.
   mt._dump = function (self)
      local tparents = assert (mt[self].parents)
      local tchildren = assert (mt[self].children)
      local roots = {}
      for _,obj in ipairs (self:getObjects ()) do
         if #obj:getParents () == 0 then
            table.insert (roots, obj)
         end
      end
      for _,root in ipairs (roots) do
         dumpGraph (root)
      end
      return
   end

   -- Exported functions ---------------------------------------------------

   -- Document::getObjects().
   mt.getObjects = function (self)
      local t = {}
      for _,v in pairs (self.object) do
         table.insert (t, v)
      end
      return t
   end

   -- Document::getObject().
   mt.getObject = function (self, id)
      return self.object[id]
   end

   -- Document::getRoot().
   mt.getRoot = function (self)
      return self.object.__root__
   end

   -- Document::getSettings ().
   mt.getSettings = function (self)
      return self.object.__settings__
   end

   -- Document::createObject().
   mt.createObject = function (self, tp, id)
      return self:_createObject (tp, id)
   end

   -- Document::getEvents().
   mt.getEvents = function (self)
      local t = {}
      for _,v in pairs (self.event) do
         table.insert (t, v)
      end
      return t
   end

   -- Document::getEvent().
   mt.getEvent = function (self, id)
      local tp, o, e = parseQualifiedId (id)
      if tp == nil then
         return nil             -- bad format
      end
      local obj = self.object[o]
      if not obj then
         return nil             -- no such object
      end
      return obj:getEvent (tp, e)
   end

   -- Document::createEvent().
   mt.createEvent = function (self, tp, obj, evtId)
      if obj == nil and evtId == nil then
         tp, objId, evtId = parseQualifiedId (tp)
      end
      if type (obj) == 'string' then
         obj = self:getObject (obj)
      end
      assert (obj.document == self)
      return self:_createEvent (tp, obj, evtId)
   end
end
