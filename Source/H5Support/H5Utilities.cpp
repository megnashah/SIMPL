/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <H5Support/H5Utilities.h>

#include "H5Fpublic.h"

// C Includes
#include <cstring>

// C++ Includes
#include <iostream>

#define CheckValidLocId(locId)                                                                                                                                                                         \
  if((locId) < 0)                                                                                                                                                                                        \
  {                                                                                                                                                                                                    \
    std::cout << "Invalid HDF Location ID: " << (locId) << std::endl;                                                                                                                                    \
    return -1;                                                                                                                                                                                         \
  }

#if defined(H5Support_NAMESPACE)
using namespace H5Support_NAMESPACE;
#endif


/**
 * Define the libraries features and file compatibility that will be used when opening
 * or creating a file
 */
#if (H5_VERS_MINOR == 8)
#define HDF5_VERSION_LIB_LOWER_BOUNDS        H5F_LIBVER_18
#define HDF5_VERSION_LIB_UPPER_BOUNDS        H5F_LIBVER_LATEST
#endif

#if (H5_VERS_MINOR == 10)
#define HDF5_VERSION_LIB_LOWER_BOUNDS        H5F_LIBVER_V18
#define HDF5_VERSION_LIB_UPPER_BOUNDS        H5F_LIBVER_V18
#endif

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t H5Utilities::createFile(const std::string& filename)
{
  H5SUPPORT_MUTEX_LOCK()

  /* Create a file access property list */
  hid_t fapl = H5Pcreate(H5P_FILE_ACCESS);

  /* Set the fapl */
  H5Pset_libver_bounds(fapl, HDF5_VERSION_LIB_LOWER_BOUNDS, HDF5_VERSION_LIB_UPPER_BOUNDS);

  /* Create a file with this fapl */
  hid_t fileId = H5Fcreate(filename.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, fapl);

  /* Close the apl object */
  H5Pclose(fapl);

  return fileId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t H5Utilities::openFile(const std::string& filename, bool readOnly)
{
  H5SUPPORT_MUTEX_LOCK()

  HDF_ERROR_HANDLER_OFF
  hid_t fileId = -1;
  if(readOnly)
  {
    fileId = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  }
  else
  {
    /* Create a file access property list */
    hid_t fapl = H5Pcreate(H5P_FILE_ACCESS);

    /* Set the fapl */
    H5Pset_libver_bounds(fapl, HDF5_VERSION_LIB_LOWER_BOUNDS, HDF5_VERSION_LIB_UPPER_BOUNDS);
    fileId = H5Fopen(filename.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);

    /* Close the apl object */
    H5Pclose(fapl);
  }

  HDF_ERROR_HANDLER_ON
  return fileId;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t H5Utilities::closeFile(hid_t& fileId)
{
  H5SUPPORT_MUTEX_LOCK()

  herr_t err = 1;
  if(fileId < 0) // fileId isn't open
  {
    return 1;
  }

  // Get the number of open identifiers of all types
  //  except files
  ssize_t num_open = H5Fget_obj_count(fileId, H5F_OBJ_DATASET | H5F_OBJ_GROUP | H5F_OBJ_DATATYPE | H5F_OBJ_ATTR | H5F_OBJ_LOCAL);
  if(num_open > 0)
  {
    std::cout << "WARNING: Some IDs weren't closed. Closing them." << std::endl;
    std::vector<hid_t> attr_ids(num_open, 0);
    H5Fget_obj_ids(fileId, H5F_OBJ_DATASET | H5F_OBJ_GROUP | H5F_OBJ_DATATYPE | H5F_OBJ_ATTR, num_open, &(attr_ids.front()));
    for(int i = 0; i < num_open; i++)
    {
      char name[1024];
      ::memset(name, 0, 1024);
      // hid_t obj_type = H5Iget_type(attr_ids[i]);
      ssize_t charsRead = H5Iget_name(attr_ids[i], name, 1024);
      if(charsRead < 0)
      {
        std::cout << "Error Trying to get the name of an hdf object that was not closed. This is probably pretty bad. " << __FILE__ << "(" << __LINE__ << ")" << std::endl;
        return -1;
      }
      std::cout << "H5 Object left open. Id=" << attr_ids[i] << " Name='" << name << "'" << std::endl;
      H5Utilities::closeHDF5Object(attr_ids[i]);
    }
  }

  err = H5Fclose(fileId);
  if(err < 0)
  {
    std::cout << "Error Closing HDF5 File. " << err << std::endl;
  }
  fileId = -1;
  return err;
}

// -----------------------------------------------------------------------------
//  Returns the full path to the object referred to by the
// -----------------------------------------------------------------------------
std::string H5Utilities::getObjectPath(hid_t loc_id, bool  /*trim*/)
{
  H5SUPPORT_MUTEX_LOCK()

  // char *obj_name;
  size_t name_size;
  name_size = 1 + H5Iget_name(loc_id, nullptr, 0);
  std::vector<char> obj_name(name_size, 0);
  H5Iget_name(loc_id, &(obj_name.front()), name_size);
  std::string objPath(&(obj_name.front()));

  if((objPath != "/") && (objPath.at(0) == '/'))
  {
    objPath.erase(0, 1);
  }

  return objPath;
}

// -----------------------------------------------------------------------------
// @brief Retrieves the HDF object type for obj_name at loc_id and stores
//    it in the parameter obj_type passed in.
// -----------------------------------------------------------------------------
herr_t H5Utilities::getObjectType(hid_t objId, const std::string& objName, int32_t* objType)
{
  H5SUPPORT_MUTEX_LOCK()

  herr_t err = 1;
  H5O_info_t obj_info{};

  err = H5Oget_info_by_name(objId, objName.c_str(), &obj_info, H5P_DEFAULT);
  if(err < 0)
  {
    return err;
  }

  (*objType) = obj_info.type;

  return err;
}

// Opens and returns the HDF object (since the HDF api requires
//  different open and close methods for different types of objects
hid_t H5Utilities::openHDF5Object(hid_t loc_id, const std::string& objName)
{
  H5SUPPORT_MUTEX_LOCK()

  int32_t obj_type;
  hid_t obj_id;
  herr_t err = 0;
  HDF_ERROR_HANDLER_OFF;
  err = getObjectType(loc_id, objName, &obj_type);
  if(err < 0)
  {
    // std::cout << "Error: Unable to get object type for object: " << objName << std::endl;
    HDF_ERROR_HANDLER_ON;
    return -1;
  }

  switch(obj_type)
  {
  case H5O_TYPE_GROUP:
    obj_id = H5Gopen(loc_id, objName.c_str(), H5P_DEFAULT);
    break;
  case H5O_TYPE_DATASET:
    obj_id = H5Dopen(loc_id, objName.c_str(), H5P_DEFAULT);
    break;
  default:
    std::cout << "Unknonwn HDF Type: " << obj_type << std::endl;
    obj_id = -1;
  }
  HDF_ERROR_HANDLER_ON;
  return obj_id;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string H5Utilities::getParentPath(hid_t objId)
{
  std::string objPath = getObjectPath(objId);
  return getParentPath(objPath);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string H5Utilities::getParentPath(const std::string& objectPath)
{
  std::string parentPath = objectPath;
  size_t start = parentPath.find_last_of('/');
  parentPath.erase(start, parentPath.size() - start);
  return parentPath;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string H5Utilities::getObjectNameFromPath(const std::string& objectPath)
{
  std::string str = objectPath;
  size_t end = str.find_last_of('/');
  str.erase(0, end+1);
  return str;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t H5Utilities::closeHDF5Object(hid_t obj_id)
{
  H5SUPPORT_MUTEX_LOCK()

  if(obj_id < 0) // Object was not valid.
  {
    return 0;
  }
  H5I_type_t obj_type;
  herr_t err = -1; // default to an error
  char name[1024];
  ::memset(name, 0, 1024);
  obj_type = H5Iget_type(obj_id);
  ssize_t charsRead = H5Iget_name(obj_id, name, 1024);
  if(charsRead < 0)
  {
    std::cout << "Error Trying to get the name of an hdf object that was not closed. This is probably pretty bad. " << __FILE__ << "(" << __LINE__ << ")" << std::endl;
    return -1;
  }

  switch(obj_type)
  {
  case H5I_FILE:
    err = H5Fclose(obj_id);
    break;
  case H5I_GROUP:
    // std::cout << "H5 Group Object left open. Id=" << obj_id  << " Name='" << name << "'" << std::endl;
    err = H5Gclose(obj_id);
    break;
  case H5I_DATASET:
    // std::cout << "H5 Dataset Object left open. Id=" << obj_id << " Name='" << name << "'" << std::endl;
    err = H5Dclose(obj_id);
    break;
  case H5I_ATTR:
    // std::cout << "H5 Attribute Object left open. Id=" << obj_id << " Name='" << name << "'" << std::endl;
    err = H5Aclose(obj_id);
    break;
  case H5I_DATATYPE:
    // std::cout << "H5 DataType Object left open. Id=" << obj_id << " Name='" << name << "'" << std::endl;
    err = H5Tclose(obj_id);
    break;
  case H5I_DATASPACE:
    // std::cout << "H5 Data Space Object left open. Id=" << obj_id << " Name='" << name << "'" << std::endl;
    err = H5Sclose(obj_id);
    break;
  default:
    // std::cout << "Error unknown HDF object for closing: " << " Name='" << name << "'" << " Object Type=" << obj_type << std::endl;
    err = -1;
  }

  return err;
}

//--------------------------------------------------------------------//
// HDF Group Methods
//--------------------------------------------------------------------//

/*! @brief Returns a std::list of std::strings containing the names
 *   of all objects attached to the group referred to by loc_id
 *
 * @parameter typeFilter is one of H5Support_GROUP, H5Support_DATASET, HDF5_TYPE,
 *  or HDF5_LINK or any combination of these using the bitwise or |
 *  command.  Or you can pass in HDF5_ANY to not filter at all
 */
herr_t H5Utilities::getGroupObjects(hid_t loc_id, int32_t typeFilter, std::list<std::string>& names)
{
  H5SUPPORT_MUTEX_LOCK()

  herr_t err = 0;
  hsize_t numObjs = 0;
  H5G_info_t group_info{};
  err = H5Gget_info(loc_id, &group_info);
  if(err < 0)
  {
    // std::cout << "Error getting number of objects for group: " << loc_id << std::endl;
    return err;
  }
  numObjs = group_info.nlinks;

  if(numObjs <= 0)
  {
    return 0; // no objects in group
  }

  size_t size = 0;
  H5O_type_t type = H5O_TYPE_NTYPES;

  for(hsize_t i = 0; i < numObjs; i++)
  {
    size = 1 + H5Lget_name_by_idx(loc_id, ".", H5_INDEX_NAME, H5_ITER_INC, i, nullptr, 0, H5P_DEFAULT);

    std::vector<char> name(size * sizeof(char), 0);

    H5Lget_name_by_idx(loc_id, ".", H5_INDEX_NAME, H5_ITER_INC, i, &(name.front()), size, H5P_DEFAULT);
    if(typeFilter == H5Support_ANY)
    {
      std::string objName(&(name.front()));
      names.push_back(objName);
    }
    else
    {
      H5O_info_t object_info{};
      err = H5Oget_info_by_name(loc_id, &(name.front()), &object_info, H5P_DEFAULT);
      if(err >= 0)
      {
        type = object_info.type;
        if(((type == H5O_TYPE_GROUP) && ((H5Support_GROUP & typeFilter) != 0)) || ((type == H5O_TYPE_DATASET) && ((H5Support_DATASET & typeFilter) != 0)))
        {
          std::string objName(&(name.front()));
          names.push_back(objName);
        }
      }
    }
  }

  return err;
}

// -----------------------------------------------------------------------------
// HDF Creation/Modification Methods
// -----------------------------------------------------------------------------
hid_t H5Utilities::createGroup(hid_t loc_id, const std::string& group)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t grp_id = -1;
  herr_t err = -1;
  H5O_info_t obj_info{};
  HDF_ERROR_HANDLER_OFF

  err = H5Oget_info_by_name(loc_id, group.c_str(), &obj_info, H5P_DEFAULT);
  //  std::cout << "H5Gget_objinfo = " << err << " for " << group << std::endl;
  if(err == 0)
  {
    grp_id = H5Gopen(loc_id, group.c_str(), H5P_DEFAULT);
  }
  else
  {
    grp_id = H5Gcreate(loc_id, group.c_str(), H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
  }
  // Turn the HDF Error handlers back on
  HDF_ERROR_HANDLER_ON

  return grp_id;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t H5Utilities::createGroupsForDataset(const std::string& datasetPath, hid_t parent)
{
  H5SUPPORT_MUTEX_LOCK()

  // Generate the internal HDF dataset path and create all the groups necessary to write the dataset
  std::string::size_type pos = 0;
  pos = datasetPath.find_last_of('/');
  // std::string parentPath;
  if(pos != 0 && pos != std::string::npos)
  {
    std::string parentPath(datasetPath.substr(0, pos));
    return H5Utilities::createGroupsFromPath(parentPath, parent);
  }
  // Make sure all the intermediary groups are in place in the HDF5 File
  return 1;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
hid_t H5Utilities::createGroupsFromPath(const std::string& pathToCheck, hid_t parent)
{
  H5SUPPORT_MUTEX_LOCK()

  hid_t gid = 1;
  herr_t err = -1;
  std::string first;
  std::string second;
  std::string path(pathToCheck); // make a copy of the input

  if(parent <= 0)
  {
    std::cout << "Bad parent Id. Returning from createGroupsFromPath" << std::endl;
    return -1;
  }
  // remove any front slash
  std::string::size_type pos = path.find_first_of('/', 0);
  if(0 == pos)
  {
    path = path.substr(1, path.size());
  }
  else if(pos == std::string::npos) // Path contains only one element
  {
    gid = H5Utilities::createGroup(parent, path);
    if(gid < 0)
    {
      std::cout << "Error creating group: " << path << " err:" << gid << std::endl;
      return gid;
    }
    err = H5Gclose(gid);
    if(err < 0)
    {
      std::cout << "Error closing group during group creation." << std::endl;
      return err;
    }
    return err; // Now return here as this was a special case.
  }

  // Remove any trailing slash
  pos = path.find_last_of('/');
  if(pos == (path.size() - 1)) // slash was in the last position
  {
    path = path.substr(0, pos);
  }

  if(path.empty())
  {
    return -1; // The path that was passed in was only a slash..
  }

  pos = path.find_first_of('/', 0);
  if(pos == std::string::npos) // Only one element in the path
  {
    gid = H5Utilities::createGroup(parent, path);
    if(gid < 0)
    {
      std::cout << "Error creating group '" << path << "' for group id " << gid << std::endl;
      return gid;
    }
    err = H5Gclose(gid);
    return err;
  }

  while(pos != std::string::npos)
  {
    first = path.substr(0, pos);
    second = path.substr(pos + 1, path.length());
    gid = H5Utilities::createGroup(parent, first);
    if(gid < 0)
    {
      std::cout << "Error creating group:" << gid << std::endl;
      return gid;
    }
    err = H5Gclose(gid);
    pos = path.find_first_of('/', pos + 1);
    if(pos == std::string::npos)
    {
      first += "/" + second;
      gid = createGroup(parent, first);
      if(gid < 0)
      {
        std::cout << "Error creating group:" << gid << std::endl;
        return gid;
      }
      err = H5Gclose(gid);
    }
  }
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string H5Utilities::extractObjectName(const std::string& path)
{
  std::string::size_type pos;
  pos = path.find_last_of('/');
  if(pos == std::string::npos || path == "/")
  {
    return path;
  }
  return path.substr(pos+1);
}

//--------------------------------------------------------------------//
// HDF Attribute Methods
//--------------------------------------------------------------------//
bool H5Utilities::probeForAttribute(hid_t loc_id, const std::string& obj_name, const std::string& attr_name)
{
  H5SUPPORT_MUTEX_LOCK()

  herr_t err = 0;
  hid_t rank;
  HDF_ERROR_HANDLER_OFF
  err = H5Lite::getAttributeNDims(loc_id, obj_name, attr_name, rank);
  HDF_ERROR_HANDLER_ON
  return err >= 0;
}

//--------------------------------------------------------------------//
// Returns a std::list of all attribute names attached to the object
//  referred to by obj_id
//--------------------------------------------------------------------//
herr_t H5Utilities::getAllAttributeNames(hid_t obj_id, std::list<std::string>& results)
{
  H5SUPPORT_MUTEX_LOCK()

  if(obj_id < 0)
  {
    return -1;
  }
  herr_t err = -1;
  hsize_t num_attrs;
  hid_t attr_id;
  size_t name_size;
  H5O_info_t object_info{};
  err = H5Oget_info(obj_id, &object_info);
  num_attrs = object_info.num_attrs;

  for(hsize_t i = 0; i < num_attrs; i++)
  {
    attr_id = H5Aopen_by_idx(obj_id, ".", H5_INDEX_NAME, H5_ITER_INC, i, H5P_DEFAULT, H5P_DEFAULT);
    name_size = 1 + H5Aget_name(attr_id, 0, nullptr);
    std::vector<char> attr_name(name_size * sizeof(char), 0);
    H5Aget_name(attr_id, name_size, &(attr_name.front()));
    results.emplace_back(&(attr_name.front()));
    err = H5Aclose(attr_id);
  }

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
herr_t H5Utilities::getAllAttributeNames(hid_t loc_id, const std::string& obj_name, std::list<std::string>& names)
{

  hid_t obj_id = -1;
  herr_t err = -1;
  names.clear();

  obj_id = openHDF5Object(loc_id, obj_name);
  if(obj_id < 0)
  {
    return static_cast<herr_t>(obj_id);
  }
  err = getAllAttributeNames(obj_id, names);
  err = closeHDF5Object(obj_id);

  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::string H5Utilities::HDFClassTypeAsStr(hid_t class_type)
{
  switch(class_type)
  {
  case H5T_INTEGER:
    return "H5T_INTEGER";
    break;
  case H5T_FLOAT:
    return "H5T_FLOAT";
    break;
  case H5T_STRING:
    return "H5T_STRING";
    break;
  case H5T_TIME:
    return "H5T_TIME";
    break;
  case H5T_BITFIELD:
    return "H5T_BITFIELD";
    break;
  case H5T_OPAQUE:
    return "H5T_OPAQUE";
    break;
  case H5T_COMPOUND:
    return "H5T_COMPOUND";
    break;
  case H5T_REFERENCE:
    return "H5T_REFERENCE";
    break;
  case H5T_ENUM:
    return "H5T_ENUM";
    break;
  case H5T_VLEN:
    return "H5T_VLEN";
    break;
  case H5T_ARRAY:
    return "H5T_ARRAY";
    break;
  default:
    return "OTHER";
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5Utilities::printHDFClassType(H5T_class_t class_type)
{
  switch(class_type)
  {
  case H5T_INTEGER:
    std::cout << "H5T_INTEGER" << std::endl;
    break;
  case H5T_FLOAT:
    std::cout << "H5T_FLOAT" << std::endl;
    break;
  case H5T_STRING:
    std::cout << "H5T_STRING" << std::endl;
    break;
  case H5T_TIME:
    std::cout << "H5T_TIME" << std::endl;
    break;
  case H5T_BITFIELD:
    std::cout << "H5T_BITFIELD" << std::endl;
    break;
  case H5T_OPAQUE:
    std::cout << "H5T_OPAQUE" << std::endl;
    break;
  case H5T_COMPOUND:
    std::cout << "H5T_COMPOUND" << std::endl;
    break;
  case H5T_REFERENCE:
    std::cout << "H5T_REFERENCE" << std::endl;
    break;
  case H5T_ENUM:
    std::cout << "H5T_ENUM" << std::endl;
    break;
  case H5T_VLEN:
    std::cout << "H5T_VLEN" << std::endl;
    break;
  case H5T_ARRAY:
    std::cout << "H5T_ARRAY" << std::endl;
    break;
  default:
    std::cout << "OTHER" << std::endl;
  }
}

// -----------------------------------------------------------------------------
//  Returns a std::string that is the name of the object at the given index
// -----------------------------------------------------------------------------
herr_t H5Utilities::objectNameAtIndex(hid_t fileId, int32_t idx, std::string& name)
{
  H5SUPPORT_MUTEX_LOCK()

  ssize_t err = -1;
  // call H5Gget_objname_by_idx with name as nullptr to get its length
  ssize_t name_len = H5Lget_name_by_idx(fileId, ".", H5_INDEX_NAME, H5_ITER_NATIVE, static_cast<hsize_t>(idx), nullptr, 0, H5P_DEFAULT);
  if(name_len < 0)
  {
    name.clear();
    return -1;
  }

  std::vector<char> buf(name_len + 1, 0);
  err = H5Lget_name_by_idx(fileId, ".", H5_INDEX_NAME, H5_ITER_NATIVE, static_cast<hsize_t>(idx), &(buf.front()), name_len + 1, H5P_DEFAULT);
  if(err < 0)
  {
    std::cout << "Error Trying to get the dataset name for index " << idx << std::endl;
    name.clear(); // Make an empty string if this fails
  }
  else
  {
    name.append(&(buf.front())); // Append the string to the given string
  }
  return static_cast<herr_t>(err);
}

// -----------------------------------------------------------------------------
// Checks the given name object to see what type of HDF5 object it is.
// -----------------------------------------------------------------------------
bool H5Utilities::isGroup(hid_t nodeId, const std::string& objName)
{
  H5SUPPORT_MUTEX_LOCK()

  bool isGroup = true;
  herr_t err = -1;
  H5O_info_t statbuf{};
  err = H5Oget_info_by_name(nodeId, objName.c_str(), &statbuf, H5P_DEFAULT);
  if(err < 0)
  {
    std::cout << "Error in methd H5Gget_objinfo" << std::endl;
    return false;
  }
  switch(statbuf.type)
  {
  case H5O_TYPE_GROUP:
    isGroup = true;
    break;
  case H5O_TYPE_DATASET:
    isGroup = false;
    break;
  case H5O_TYPE_NAMED_DATATYPE:
    isGroup = false;
    break;
  default:
    isGroup = false;
  }
  return isGroup;
}
