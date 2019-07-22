#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <vector>

using namespace std;

struct packet_t {
  uint time_of_arrival;
  uint processing_time;
};

// For each packet output either the moment of time
// (in milliseconds) when the processor began processing
// it or −1 if the packet was dropped (output the answers for
// the packets in the same order as the packets are given in the input).
vector<int> simulate(vector<packet_t> packets, uint buffer_size) {
  assert(std::is_sorted(packets.begin(), packets.end(), [](auto& p1, auto& p2) {
    return p1.time_of_arrival < p2.time_of_arrival;
  }));

  vector<int> packets_processing_begins(packets.size());

  // imagine first, that buffer is super long, unlimited.

  // assuming that packets are sorted by time of arrival,
  // packet time_of_arrival can be treated as current time in our simulation.
  // so when we process packet X we use its time_of_arrival as current
  // timepoint. so we can see which packets are already finished at this time
  // and which still are being processed.

  list<uint> finish_time_queue;
  uint current_time = 0;
  for (uint i = 0; i < packets.size(); ++i) {
    auto& packet = packets[i];

    auto current_time = packet.time_of_arrival;

    // check which items have been finished by current time
    while (!finish_time_queue.empty() &&
           finish_time_queue.front() <= current_time) {
      finish_time_queue.pop_front();
    }

    if (finish_time_queue.size() >= buffer_size) {
      packets_processing_begins[i] = -1;
      continue;
    }

    if (finish_time_queue.empty()) {
      // this task starts immidiately
      packets_processing_begins[i] = packet.time_of_arrival;
      finish_time_queue.push_back(packet.time_of_arrival +
                                  packet.processing_time);
    } else {
      auto last_one_finish_time = finish_time_queue.back();
      uint this_packet_finish_time =
          last_one_finish_time + packet.processing_time;
      finish_time_queue.push_back(this_packet_finish_time);

      packets_processing_begins[i] = last_one_finish_time;
    }
  }

  return packets_processing_begins;
}

int main() {
  uint S, n;
  cin >> S;  // size of the buffer
  cin >> n;  // number of packets

  vector<packet_t> packets(n);
  for (auto& packet : packets) {
    cin >> packet.time_of_arrival;
    cin >> packet.processing_time;
  }

  for (auto start_time : simulate(packets, S)) {
    cout << start_time << endl;
  }
}
