/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_STATUS_HPP
#define IROHA_STATUS_HPP

#include <optional>
#include <cstdint>
#include <string>
#include <rapidjson/stringbuffer.h>

#include "consensus/round.hpp"

namespace iroha {

  struct IrohaStatus {
    std::optional<uint64_t> memory_consumption;
    std::optional<consensus::Round> last_round;
    std::optional<bool> is_syncing;
    std::optional<bool> is_healthy;
  };

  struct IrohaStoredStatus {
    IrohaStatus status;
    rapidjson::StringBuffer serialized_status;
  };

}

#endif  // IROHA_STATUS_HPP
