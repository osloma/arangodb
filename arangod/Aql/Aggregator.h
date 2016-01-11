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

#ifndef ARANGOD_AQL_AGGREGATOR_H
#define ARANGOD_AQL_AGGREGATOR_H 1

#include "Basics/Common.h"
#include "Aql/AqlValue.h"
#include "Basics/Exceptions.h"
#include "Basics/JsonHelper.h"
#include "Utils/AqlTransaction.h"

struct TRI_document_collection_t;

namespace triagens {
namespace arango {
  class AqlTransaction;
}
namespace aql {

struct Aggregator {
  Aggregator(triagens::arango::AqlTransaction* trx) : trx(trx) { }
  virtual ~Aggregator() = default;
  virtual char const* name() const = 0;
  virtual void reset() = 0;
  virtual void reduce(AqlValue const&, struct TRI_document_collection_t const*) = 0;
  virtual AqlValue getValue() = 0;

  static Aggregator* fromTypeString(triagens::arango::AqlTransaction*, std::string const&);
  static Aggregator* fromJson(triagens::arango::AqlTransaction*, triagens::basics::Json const&,  
                              char const*);

  static bool isSupported(std::string const&);
  
  triagens::arango::AqlTransaction* trx;
};

struct AggregatorLength final : public Aggregator {
  AggregatorLength(triagens::arango::AqlTransaction* trx) : Aggregator(trx), count(0) { }

  char const* name() const override final {
    return "LENGTH";
  }

  void reset() override final;
  void reduce(AqlValue const&, struct TRI_document_collection_t const*) override final;
  AqlValue getValue() override final;

  uint64_t count;
};

struct AggregatorMin final : public Aggregator {
  AggregatorMin(triagens::arango::AqlTransaction* trx) : Aggregator(trx), value(), coll(nullptr) { }
  
  ~AggregatorMin();

  char const* name () const override final {
    return "MIN";
  }

  void reset() override final;
  void reduce(AqlValue const&, struct TRI_document_collection_t const*) override final;
  AqlValue getValue() override final;

  AqlValue value;
  struct TRI_document_collection_t const* coll;
};

struct AggregatorMax final : public Aggregator {
  AggregatorMax(triagens::arango::AqlTransaction* trx) : Aggregator(trx), value(), coll(nullptr) { }
  
  ~AggregatorMax();

  char const* name () const override final {
    return "MAX";
  }

  void reset() override final;
  void reduce(AqlValue const&, struct TRI_document_collection_t const*) override final;
  AqlValue getValue() override final;
  
  AqlValue value;
  struct TRI_document_collection_t const* coll;
};

struct AggregatorSum final : public Aggregator {
  AggregatorSum(triagens::arango::AqlTransaction* trx) : Aggregator(trx), sum(0.0), invalid(false) { }
  
  ~AggregatorSum();

  char const* name () const override final {
    return "SUM";
  }

  void reset() override final;
  void reduce(AqlValue const&, struct TRI_document_collection_t const*) override final;
  AqlValue getValue() override final;
  
  double sum;
  bool invalid;
};

struct AggregatorAverage final : public Aggregator {
  AggregatorAverage(triagens::arango::AqlTransaction* trx) : Aggregator(trx), count(0), sum(0.0), invalid(false) { }
  
  ~AggregatorAverage();

  char const* name () const override final {
    return "AVERAGE";
  }

  void reset() override final;
  void reduce(AqlValue const&, struct TRI_document_collection_t const*) override final;
  AqlValue getValue() override final;
  
  uint64_t count;
  double sum;
  bool invalid;
};


}  // namespace triagens::aql
}  // namespace triagens

#endif

