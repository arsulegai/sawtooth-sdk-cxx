/* Copyright 2017 Intel Corporation

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
------------------------------------------------------------------------------*/
#pragma once

#include <map>
#include <memory>
#include <string>

#include "sawtooth_sdk.h"
#include "sawtooth/transaction_handler.h"
#include "sawtooth/message_dispatcher.h"

namespace sawtooth {

// This is the version used by SDK to match if validator supports feature
// it requested during registration. It should only be incremented when
// there are changes in TpRegisterRequest. Remember to sync this
// information in validator if changed.
// Note: SdkProtocolVersion is the highest version the SDK supports
enum class FeatureVersion: uint32_t {
    FeatureUnused = 0,
    FeatureCustomHeaderStyle = 1,
    SdkProtocolVersion = 1,
};

static const unsigned int FeatureVersionToUnsignedInt(
        const FeatureVersion& feature_version) {
    // Default value
    unsigned int value = 0;
    switch (feature_version) {
        case FeatureVersion::FeatureUnused:
            value = 0;
        case FeatureVersion::FeatureCustomHeaderStyle:
            value = 1;
        // case FeatureVersion::SdkProtocolVersion:
        default:
            value = 1;
    }
    return value;
}

// The main processing class for the Sawtooth SDK.
class TransactionProcessorImpl: public TransactionProcessor {
 public:
    // Pass a valid ZMQ connection string for the Validator interconnect
    // address
    explicit TransactionProcessorImpl(const std::string& connection_string);
    virtual ~TransactionProcessorImpl();
    // Called to register the TransactionHandlers defined by your Transaction
    // Processor.  All the TransactionHandler objects must be registered
    // before run is called.
    void RegisterHandler(TransactionHandlerUPtr handler);

    // Sets the TpProcessRequest header style, default set to EXPANDED
    void SetHeaderStyle(TpRequestHeaderStyle style);

    // The main entry point for the TransactionProcessor. It will not return
    // until the TransactionProcessor shuts down.
    void Run();

 private:
    void Register();
    void UnRegister();
    void HandleProcessingRequest(const void* msg,
        size_t msg_size,
        const std::string& correlation_id);

    bool run;
    std::string connection_string;
    MessageDispatcher message_dispatcher;
    MessageStreamPtr response_stream;

    std::map<std::string, TransactionHandlerPtr> handlers;
    FeatureVersion highest_sdk_feature_requested;
    TpRegisterRequest_TpProcessRequestHeaderStyle header_style;
};

}  // namespace sawtooth
