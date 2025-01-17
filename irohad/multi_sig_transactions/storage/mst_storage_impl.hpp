/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef IROHA_MST_STORAGE_IMPL_HPP
#define IROHA_MST_STORAGE_IMPL_HPP

#include <memory>
#include <unordered_map>

#include "logger/logger_fwd.hpp"
#include "multi_sig_transactions/hash.hpp"
#include "multi_sig_transactions/storage/mst_storage.hpp"

namespace iroha {
  class MstStorageStateImpl : public MstStorage {
   private:
    // -----------------------------| private API |-----------------------------

    /**
     * Return state of a peer by its public key. If state doesn't exist, create
     * new empty state and return it.
     * @param target_peer_key - public key of the peer for searching
     * @return valid iterator for state of peer
     */
    auto getState(
        shared_model::interface::types::PublicKeyHexStringView target_peer_key);

   public:
    // ----------------------------| interface API |----------------------------
    MstStorageStateImpl(CompleterType const &completer,
                        logger::LoggerPtr mst_state_logger,
                        logger::LoggerPtr log);

    MstStorageStateImpl(MstStorageStateImpl const &) = delete;
    MstStorageStateImpl &operator=(MstStorageStateImpl const &) = delete;

    auto applyImpl(
        shared_model::interface::types::PublicKeyHexStringView target_peer_key,
        const MstState &new_state)
        -> decltype(apply(target_peer_key, new_state)) override;

    auto updateOwnStateImpl(const DataType &tx)
        -> decltype(updateOwnState(tx)) override;

    auto extractExpiredTransactionsImpl(const TimeType &current_time)
        -> decltype(extractExpiredTransactions(current_time)) override;

    auto getDiffStateImpl(
        shared_model::interface::types::PublicKeyHexStringView target_peer_key,
        const TimeType &current_time)
        -> decltype(getDiffState(target_peer_key, current_time)) override;

    auto whatsNewImpl(MstState const &new_state) const
        -> decltype(whatsNew(new_state)) override;

    bool batchInStorageImpl(const DataType &batch) const override;

    void processFinalizedTransactionImpl(
        shared_model::interface::types::HashType const &hash) override;

    std::tuple<size_t, size_t> countBatchesTxs() const {
      std::lock_guard lk(mutex_);
      return {own_state_.count_batches(), own_state_.count_transactions()};
    }

   private:
    // ---------------------------| private fields |----------------------------

    const CompleterType completer_;
    struct StringViewOrString {
      std::string s;
      std::string_view v;

      explicit StringViewOrString(std::string_view v) : v(v) {}
      explicit StringViewOrString(std::string s) : s(s), v(this->s) {}

      StringViewOrString(StringViewOrString const &o)
          : s(o.s), v(not this->s.empty() ? this->s : o.v) {}
      StringViewOrString(StringViewOrString &&o) noexcept
          : s(std::move(o).s),
            v(not this->s.empty() ? this->s : std::move(o).v) {}

      bool operator==(StringViewOrString const &x) const {
        return v == x.v;
      }

      struct Hash {
        std::size_t operator()(StringViewOrString const &x) const {
          return std::hash<std::string_view>()(x.v);
        }
      };
    };

    std::unordered_map<StringViewOrString, MstState, StringViewOrString::Hash>
        peer_states_;
    MstState own_state_;
    logger::LoggerPtr mst_state_logger_;
  };
}  // namespace iroha

#endif  // IROHA_MST_STORAGE_IMPL_HPP
