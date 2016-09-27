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

#ifndef ARANGOD_VOCBASE_COLLECTION_REVISIONS_CACHE_H
#define ARANGOD_VOCBASE_COLLECTION_REVISIONS_CACHE_H 1

#include "Basics/Common.h"
#include "Basics/ReadWriteLock.h"
#include "VocBase/ManagedDocumentResult.h"
#include "VocBase/ReadCache.h"
#include "VocBase/voc-types.h"

namespace arangodb {
namespace wal {
class Logfile;
}

class PhysicalCollection;
class RevisionCacheChunkAllocator;

class CollectionRevisionsCache {
 public:
  explicit CollectionRevisionsCache(PhysicalCollection*, RevisionCacheChunkAllocator*);
  ~CollectionRevisionsCache();
  
 public:
  void clear();

  // look up a revision
  bool lookupRevision(ManagedDocumentResult& result, TRI_voc_rid_t revisionId);
  
  bool lookupRevision(ManagedMultiDocumentResult& result, TRI_voc_rid_t revisionId);

  // insert from chunk
  void insertRevision(TRI_voc_rid_t revisionId, RevisionCacheChunk* chunk, uint32_t offset, uint32_t version);
  
  // insert from WAL
  void insertRevision(TRI_voc_rid_t revisionId, wal::Logfile* logfile, uint32_t offset);

  // remove a revision
  void removeRevision(TRI_voc_rid_t revisionId);

  // remove multiple revisions
  void removeRevisions(std::vector<TRI_voc_rid_t> const& revisions);

 private:
  uint8_t const* readFromEngine(TRI_voc_rid_t revisionId);

 private:
  arangodb::basics::ReadWriteLock _lock; 

  std::unordered_map<TRI_voc_rid_t, RevisionCacheEntry> _revisions;

  PhysicalCollection* _physical;

  ReadCache _readCache;
};

} // namespace arangodb

#endif
