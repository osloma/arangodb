////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#ifndef ARANGOD_VOCBASE_REVISION_CACHE_H
#define ARANGOD_VOCBASE_REVISION_CACHE_H 1

#include "Basics/Common.h"
#include "Basics/Mutex.h"
#include "VocBase/voc-types.h"

#include <list>

namespace arangodb {
class RevisionCacheChunk;
class RevisionCacheChunkAllocator;

class DocumentPosition {
  enum class PositionType : uint32_t {
    DATAFILE,
    CHUNK
  };

 public:
  DocumentPosition(TRI_voc_fid_t datafileId, uint32_t offset) 
      : _location(datafileId), _offset(offset), _type(PositionType::DATAFILE) {}
  
  DocumentPosition(RevisionCacheChunk* chunk, uint32_t offset) 
      : _location(chunk), _offset(offset), _type(PositionType::CHUNK) {}

  ~DocumentPosition() = default;

 private:
  union Location {
    Location() = delete;
    explicit Location(RevisionCacheChunk* chunk) : chunk(chunk) {}
    explicit Location(TRI_voc_fid_t datafileId) : datafileId(datafileId) {}
    
    TRI_voc_fid_t datafileId;
    RevisionCacheChunk* chunk;
  };
  
  Location _location;
  uint32_t _offset;
  PositionType _type;
};

class CollectionRevisionCache {
 public:
  explicit CollectionRevisionCache(RevisionCacheChunkAllocator* allocator);
  ~CollectionRevisionCache();
  
 public:
  bool insertFromWal(TRI_voc_rid_t revisionId, TRI_voc_fid_t datafileId, uint32_t offset);

  bool insertFromChunk(TRI_voc_rid_t revisionId, RevisionCacheChunk* chunk, uint32_t offset);

  bool remove(TRI_voc_rid_t revisionId);

  bool garbageCollect(size_t maxChunksToClear);

 private:
  uint8_t* store(uint8_t const* data, size_t size);

 private:
  RevisionCacheChunkAllocator* _allocator;
  
  arangodb::Mutex _writeMutex;
  std::unordered_map<TRI_voc_rid_t, DocumentPosition> _positions;
 
  RevisionCacheChunk* _writeChunk; 
  std::list<RevisionCacheChunk*> _fullChunks;
};

} // namespace arangodb

#endif
