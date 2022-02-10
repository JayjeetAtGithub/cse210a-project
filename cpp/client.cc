#include <iostream>

#include <arrow/api.h>
#include <arrow/flight/api.h>
#include <arrow/filesystem/api.h>
#include <arrow/ipc/api.h>
#include <arrow/io/api.h>


struct ConnectionInfo {
  std::string host;
  int32_t port;
};

arrow::Result<std::unique_ptr<arrow::flight::FlightClient>> ConnectToFlightServer(ConnectionInfo info) {
  arrow::flight::Location location;
  ARROW_RETURN_NOT_OK(
      arrow::flight::Location::ForGrpcTcp(info.host, info.port, &location));

  std::unique_ptr<arrow::flight::FlightClient> client;
  ARROW_RETURN_NOT_OK(arrow::flight::FlightClient::Connect(location, &client));
  std::cout << "Connected to " << location.ToString() << std::endl;
  return client;
}

int main(int argc, char *argv[]) {
  // Get connection info from user input
  ConnectionInfo info;
  info.host = argv[1];
  info.port = (int32_t)std::stoi(argv[2]);

  // Connect to flight server
  auto client = ConnectToFlightServer(info).ValueOrDie();
  auto descriptor = arrow::flight::FlightDescriptor::Path({"16MB.uncompressed.parquet"});

  // Get flight info
  std::unique_ptr<arrow::flight::FlightInfo> flight_info;
  client->GetFlightInfo(descriptor, &flight_info);
  std::cout << flight_info->descriptor().ToString() << std::endl;

  std::cout << "=== Schema ===" << std::endl;
  std::shared_ptr<arrow::Schema> info_schema;
  arrow::ipc::DictionaryMemo dictionary_memo;
  flight_info->GetSchema(&dictionary_memo, &info_schema);
  std::cout << info_schema->ToString() << std::endl;
  std::cout << "==============" << std::endl;

  std::unique_ptr<arrow::flight::FlightStreamReader> stream;
  client->DoGet(flight_info->endpoints()[0].ticket, &stream);
  std::shared_ptr<arrow::Table> table;
  stream->ReadAll(&table);
  std::cout << table->ToString() << std::endl;
}
