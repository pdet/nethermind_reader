#include <iostream>
#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <rocksdb/slice.h>
#include <rocksdb/status.h>
#include <rocksdb/iterator.h>
#include "include/rlp.h"


struct BlockHeader {
    dev::h256 parentHash;
    dev::h256 unclesHash;
    dev::h256 beneficiary;
    dev::h256 stateRoot;
    dev::h256 transactionsRoot;
    dev::h256 receiptsRoot;
       std::vector<uint8_t> bloom;
    dev::u256 difficulty;
    int64_t number;
    int64_t gasLimit;
    int64_t gasUsed;
    uint64_t timestamp;
    std::vector<uint8_t> extraData;
    std::optional<dev::h256> mixHash;
    std::optional<uint64_t> nonce;
    std::optional<int64_t> auRaStep;
    std::optional<std::vector<uint8_t>> auRaSignature;
    std::optional<dev::u256> baseFeePerGas;
    std::optional<dev::h256> withdrawalsRoot;
    std::optional<uint64_t> blobGasUsed;
    std::optional<uint64_t> excessBlobGas;
    std::optional<dev::h256> parentBeaconBlockRoot;
};



BlockHeader decodeBlockHeader(const dev::RLP& rlp) {
    BlockHeader header;

    if (rlp.isNull()) {
        throw std::invalid_argument("RLP data is null");
    }

    int headerSequenceLength = rlp.itemCount();
    int headerCheck = rlp.actualSize();

    header.parentHash = rlp[0].toHash<dev::h256>();
    header.unclesHash = rlp[1].toHash<dev::h256>();
    header.beneficiary = rlp[2].toHash<dev::h256>();
    header.stateRoot = rlp[3].toHash<dev::h256>();
    header.transactionsRoot = rlp[4].toHash<dev::h256>();
    header.receiptsRoot = rlp[5].toHash<dev::h256>();
    header.bloom = rlp[6].toBytes();
    header.difficulty = rlp[7].toInt<dev::u256>();
    header.number = rlp[8].toInt<int64_t>();
    header.gasLimit = rlp[9].toInt<int64_t>();
    header.gasUsed = rlp[10].toInt<int64_t>();
    header.timestamp = rlp[11].toInt<uint64_t>();
    header.extraData = rlp[12].toBytes();

    if (rlp[13].size() == dev::h256::size) {
        header.mixHash = rlp[13].toHash<dev::h256>();
       // header.nonce = rlp[14].toInt<uint64_t>();
    } else {
        header.auRaStep = rlp[13].toInt<int64_t>();
        header.auRaSignature = rlp[14].toBytes();
    }

    if (rlp.actualSize() != headerCheck) {
        header.baseFeePerGas = rlp[15].toInt<dev::u256>();
    }

    int itemsRemaining = rlp.itemCount() - 16;
    if (itemsRemaining > 0 && rlp[16].size() == dev::h256::size) {
        header.withdrawalsRoot = rlp[16].toHash<dev::h256>();

        if (itemsRemaining >= 3 && rlp.actualSize() != headerCheck) {
            header.blobGasUsed = rlp[17].toInt<uint64_t>();
            header.excessBlobGas = rlp[18].toInt<uint64_t>();
        }

        if (itemsRemaining == 4 && rlp.actualSize() != headerCheck) {
            header.parentBeaconBlockRoot = rlp[19].toHash<dev::h256>();
        }
    }

    return header;
}



int main() {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = false;

   std::string db_path = "/Users/holanda/Documents/Projects/ethereum/data/nethermind_db/mainnet/blocks";


    rocksdb::Status status = rocksdb::DB::Open(options, db_path, &db);
    if (!status.ok()) {
        std::cerr << "Unable to open RocksDB: " << status.ToString() << std::endl;
        return 1;
    }

    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        std::string value = it->value().ToString();
        auto value_bytes = reinterpret_cast<const uint8_t*>(value.data());
        dev::RLP rlp_key(reinterpret_cast<const uint8_t*>(key.data()), key.size(), dev::RLP::LaissezFaire);
        dev::RLP rlp_value(value_bytes, value.size(), dev::RLP::LaissezFaire);

        uint64_t blockNumber = rlp_key.convert<uint64_t>(dev::RLP::LaissezFaire);
        std::cout << "Block Number: " << blockNumber << std::endl;

        // Decode block header
        BlockHeader header = decodeBlockHeader(rlp_value[0]);
       std::cout << "Parent Hash: " << header.parentHash << std::endl;
        std::cout << "Uncles Hash: " << header.unclesHash << std::endl;
        std::cout << "Beneficiary: " << header.beneficiary << std::endl;
        std::cout << "State Root: " << header.stateRoot << std::endl;
        std::cout << "Transactions Root: " << header.transactionsRoot << std::endl;
        std::cout << "Receipts Root: " << header.receiptsRoot << std::endl;
        // std::cout << "Bloom: " << header.bloom << std::endl;
        std::cout << "Difficulty: " << header.difficulty << std::endl;
        std::cout << "Number: " << header.number << std::endl;
        std::cout << "Gas Limit: " << header.gasLimit << std::endl;
        std::cout << "Gas Used: " << header.gasUsed << std::endl;
        std::cout << "Timestamp: " << header.timestamp << std::endl;
        std::cout << "Extra Data: " << header.extraData.size() << " bytes" << std::endl;

        if (header.mixHash) {
            std::cout << "Mix Hash: " << *header.mixHash << std::endl;
        }

        if (header.nonce) {
            std::cout << "Nonce: " << *header.nonce << std::endl;
        }

        if (header.auRaStep) {
            std::cout << "AuRa Step: " << *header.auRaStep << std::endl;
        }

        if (header.auRaSignature) {
            std::cout << "AuRa Signature: " << header.auRaSignature->size() << " bytes" << std::endl;
        }

        if (header.baseFeePerGas) {
            std::cout << "Base Fee Per Gas: " << *header.baseFeePerGas << std::endl;
        }

        if (header.withdrawalsRoot) {
            std::cout << "Withdrawals Root: " << *header.withdrawalsRoot << std::endl;
        }

        if (header.blobGasUsed) {
            std::cout << "Blob Gas Used: " << *header.blobGasUsed << std::endl;
        }

        if (header.excessBlobGas) {
            std::cout << "Excess Blob Gas: " << *header.excessBlobGas << std::endl;
        }

        if (header.parentBeaconBlockRoot) {
            std::cout << "Parent Beacon Block Root: " << *header.parentBeaconBlockRoot << std::endl;
        }

        // Process transactions and other parts if needed
    }

    // Clean up
    delete it;
    delete db;

    return 0;
}
