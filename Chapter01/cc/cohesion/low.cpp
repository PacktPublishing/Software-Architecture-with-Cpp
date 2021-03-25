#include <vector>

using Result = int;
using Results = std::vector<int>;
using WorkItem = int;
using WorkBatch = std::vector<int>;
struct Listener;

class CachingProcessor {
 public:
  Result process(WorkItem work) { return {}; }
  Results processBatch(WorkBatch batch) { return {}; }
  void addListener(const Listener &listener) {}
  void removeListener(const Listener &listener) {}

 private:
  void addToCache(const WorkItem &work, const Result &result) {}
  void findInCache(const WorkItem &work) {}
  void limitCacheSize(std::size_t size) {}
  void notifyListeners(const Result &result) {}
  // ...
};
