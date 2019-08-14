#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <tuple>
#include <vector>

using namespace std;

struct parallel_processing_t {
  parallel_processing_t(uint64_t thread_id, uint64_t start_time)
      : thread_id(thread_id), start_time(start_time) {}

  parallel_processing_t() : thread_id(-1), start_time(-1) {}

  bool operator==(parallel_processing_t other) const {
    return thread_id == other.thread_id && start_time == other.start_time;
  }

  uint64_t thread_id, start_time;
};

struct active_job_t {
  active_job_t(uint64_t processor, uint64_t finish_time)
      : processor(processor), finish_time(finish_time) {}
  active_job_t() : processor(-1), finish_time(-1) {}

  bool operator<(active_job_t other) const {
    return std::tie(finish_time, processor) <
           std::tie(other.finish_time, other.processor);
  }

  uint64_t processor, finish_time;
};

size_t parent(size_t i) { return (i - 1) / 2; }

size_t left_child(size_t i) { return i * 2 + 1; }

size_t right_child(size_t i) { return i * 2 + 2; }

void sift_up(vector<active_job_t> &heap, size_t i) {
  while (i > 0 && heap[i] < heap[parent(i)]) {
    std::swap(heap[i], heap[parent(i)]);
    i = parent(i);
  }
}

void sift_down(vector<active_job_t> &heap, size_t i) {
  while (i < heap.size()) {
    size_t min_index = i;

    size_t l = left_child(i);
    if (l < heap.size() && heap[l] < heap[min_index]) {
      min_index = l;
    }
    size_t r = right_child(i);
    if (r < heap.size() && heap[r] < heap[min_index]) {
      min_index = r;
    }
    if (i != min_index) {
      std::swap(heap[i], heap[min_index]);
      i = min_index;
    } else {
      // on its place.
      break;
    }
  }
}

void heap_insert(vector<active_job_t> &heap, active_job_t v) {
  heap.push_back(v);
  sift_up(heap, heap.size() - 1);
}

active_job_t heap_extract(vector<active_job_t> &heap) {
  assert(heap.size() > 0);
  auto x = heap[0];
  std::swap(heap[heap.size() - 1], heap[0]);
  heap.resize(heap.size() - 1);
  sift_down(heap, 0);
  return x;
}

// What to Do
// Think about the sequence of events when one of the threads becomes free (at
// the start and later after completing some job). How to apply priority queue
// to simulate processing of these events in the required order? Remember to
// consider the case when several threads become free simultaneously.
vector<parallel_processing_t>
simulate_parallel_processing(uint64_t processors_n, vector<uint64_t> jobs) {
  vector<parallel_processing_t> result(jobs.size());
  vector<active_job_t> active_jobs;
  // before processing, all processors are ready
  for (uint64_t pi = 0; pi != processors_n; ++pi) {
    active_jobs.emplace_back(pi, 0);
  }

  size_t next_job = 0;
  while (next_job < result.size()) {
    assert(next_job < result.size());
    auto job = heap_extract(active_jobs);
    // job.processor is ready for next task and current time is job.finish_time
    heap_insert(active_jobs,
                active_job_t{job.processor, job.finish_time + jobs[next_job]});
    result[next_job] = {job.processor, job.finish_time};
    next_job++;
  }

  return result;
}

// Taken from starter files
vector<parallel_processing_t> parallel_processing_naive(uint64_t processors_n,
                                                        vector<uint64_t> jobs) {
  vector<int> start_times;
  vector<int> assigned_workers;

  assigned_workers.resize(jobs.size());
  start_times.resize(jobs.size());
  vector<long long> next_free_time(processors_n, 0);
  for (int i = 0; i < jobs.size(); ++i) {
    int duration = jobs[i];
    int next_worker = 0;
    for (int j = 0; j < processors_n; ++j) {
      if (next_free_time[j] < next_free_time[next_worker])
        next_worker = j;
    }
    assigned_workers[i] = next_worker;
    start_times[i] = next_free_time[next_worker];
    next_free_time[next_worker] += duration;
  }

  vector<parallel_processing_t> result(assigned_workers.size());
  assert(start_times.size() == assigned_workers.size());
  for (size_t i = 0; i < start_times.size(); ++i) {
    result[i].thread_id = assigned_workers[i];
    result[i].start_time = start_times[i];
  }
  return std::move(result);
}

void stress_test() {
  for (uint test_num = 0; test_num < 101; ++test_num) {
    auto processors_num = rand() % 15 + 1;
    auto jobs_num = rand() % 100 + 5;
    vector<uint64_t> jobs(jobs_num);
    for (auto &j : jobs) {
      j = rand() % 40 + 1;
    }

    auto naive = parallel_processing_naive(processors_num, jobs);
    auto fast = simulate_parallel_processing(processors_num, jobs);

    if (naive != fast) {
      cout << "DIFF:" << endl;

      cout << "naive:" << endl;
      for (auto x : naive) {
        cout << x.thread_id << " " << x.start_time << endl;
      }
      cout << "fast:" << endl;
      for (auto x : fast) {
        cout << x.thread_id << " " << x.start_time << endl;
      }
      break;
    } else {
      cout << "PASSED: " << test_num << endl;
    }
  }
}

#ifdef _TESTS
int main() {
  cout << "running stress tests" << endl;
  stress_test();
}
#else

int main() {
  uint n;
  cin >> n;

  uint m;
  cin >> m;

  vector<uint64_t> job_processing_times(m);
  for (auto &x : job_processing_times) {
    cin >> x;
  }

  for (auto x : simulate_parallel_processing(n, job_processing_times)) {
    cout << x.thread_id << " " << x.start_time << endl;
  }
}
#endif // _TESTS