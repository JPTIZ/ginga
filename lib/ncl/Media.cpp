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

#include "ginga-internal.h"
#include "Media.h"

GINGA_NCL_BEGIN

static map<string, string> mimetab =
{
  {"ac3", "audio/ac3"},
  {"avi", "video/x-msvideo"},
  {"bmp", "image/bmp"},
  {"bpg", "image/x-bpg"},
  {"class", "application/x-ginga-NCLet"},
  {"css", "text/css"},
  {"gif", "image/gif"},
  {"htm", "text/html"},
  {"html", "text/html"},
  {"jpeg", "image/jpeg"},
  {"jpg", "image/jpeg"},
  {"lua", "application/x-ginga-NCLua"},
  {"mov", "video/quicktime"},
  {"mp2", "audio/mp2"},
  {"mp3", "audio/mp3"},
  {"mp4", "video/mp4"},
  {"mpa", "audio/mpa"},
  {"mpeg", "video/mpeg"},
  {"mpg", "video/mpeg"},
  {"mpv", "video/mpv"},
  {"ncl", "application/x-ginga-ncl"},
  {"oga", "audio/ogg"},
  {"ogg", "audio/ogg"},
  {"ogv", "video/ogg"},
  {"opus", "audio/ogg"},
  {"png", "image/png"},
  {"smil", "application/smil"},
  {"spx", "audio/ogg"},
  {"srt", "text/srt"},
  {"ssml", "application/ssml+xml"},
  {"svg", "image/svg+xml"},
  {"svgz", "image/svg+xml"},
  {"ts", "video/mpeg"},
  {"txt", "text/plain"},
  {"wav", "audio/basic"},
  {"webp", "image/x-webp"},
  {"wmv", "video/x-ms-wmv"},
  {"xlet", "application/x-ginga-NCLet"},
  {"xlt", "application/x-ginga-NCLet"},
  {"xml", "text/xml"},
};

static bool
mime_table_index (const string &key, string *result)
{
  map<string, string>::iterator it;
  if ((it = mimetab.find (key)) == mimetab.end ())
    return false;
  tryset (result, it->second);
  return true;
}


// Public.

/**
 * @brief Creates a new media.
 * @param id Media id.
 * @param settings True if media is settings node.
 */
Media::Media (const string &id, bool settings)
  : Node (id)
{
  _isSettings = settings;
  _src = "";
  _mimetype = (settings) ? "application/x-ginga-settings" : "";
  _descriptor = nullptr;
}

/**
 * @brief Destroys media.
 */
Media::~Media ()
{
  if (_descriptor != nullptr)
    delete _descriptor;
  _instances.clear ();
}

/**
 * @brief Tests if media is settings node.
 */
bool
Media::isSettings ()
{
  return _isSettings;
}

/**
 * @brief Gets media mime-type.
 */
string
Media::getMimeType ()
{
  return _mimetype;
}

/**
 * @brief Gets media source.
 */
string
Media::getSrc ()
{
  return _src;
}

/**
 * @brief Sets media source.
 */
void
Media::setSrc (const string &src)
{
  string type, extension;
  string::size_type index, len;

  type = "";
  if (src == "")
    goto done;

  index = src.find_last_of (".");
  if (index != std::string::npos)
    {
      index++;
      len = src.length ();
      if (index < len)
        {
          extension = src.substr (index, (len - index));
          if (extension != "")
            mime_table_index (extension, &type);
        }
    }

 done:
  _src = src;
  _mimetype = type;
}

/**
 * @brief Gets media descriptor.
 */
Descriptor *
Media::getDescriptor ()
{
  return _descriptor;
}

/**
 * @brief Sets media descriptor.  (Can only be called once.)
 */
void
Media::setDescriptor (Descriptor *descriptor)
{
  g_assert_null (_descriptor);
  g_assert_nonnull (descriptor);
  _descriptor = descriptor;
}

/**
 * @brief Add media reference.
 * @param node Refer node.
 */
void
Media::addSameInstance (Refer *node)
{
  g_assert_nonnull (node);
  _instances.insert (node);
}

/**
 * @brief Get media references.
 */
const set<Refer *> *
Media::getInstSameInstances ()
{
  return &_instances;
}

GINGA_NCL_END