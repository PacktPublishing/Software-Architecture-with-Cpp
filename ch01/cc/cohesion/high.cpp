#include <vector>

using Result = int;
using Results = std::vector<int>;
using WorkItem = int;
using WorkBatch = std::vector<int>;
struct Listener;

class WorkResultsCache {
 public:
  void addToCache(const WorkItem &work, const Result &result) {}
  void findInCache(const WorkItem &work) {}
  void limitCacheSize(std::size_t size) {}

 private:
  // ...
};

class ResultNotifier {
 public:
  void addListener(const Listener &listener) {}
  void removeListener(const Listener &listener) {}
  void notify(const Result &result) {}

 private:
  // ...
};

class CachingProcessor {
 public:
  explicit CachingProcessor(ResultNotifier &notifier) {}
  Result process(WorkItem work) { return {}; }
  Results processBatch(WorkBatch batch) { return {}; }

 private:
  WorkResultsCache cache_;
  ResultNotifier notifier_;
  // ...
};
