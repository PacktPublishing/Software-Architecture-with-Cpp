class CachingProcessor {
public:
  explicit CachingProcessor(ResultNotifier &notifier);
  Result process(WorkItem work);
  Results processBatch(WorkBatch batch);

private:
  WorkResultsCache cache_;
  ResultNotifier notifier_;
  // ...
};

class WorkResultsCache {
public:
  void addToCache(const WorkItem &work, const Result &result);
  void findInCache(const WorkItem &work);
  void limitCacheSize(std::size_t size);

private:
  // ...
};

class ResultNotifier {
public:
  void addListener(const Listener &listener);
  void removeListener(const Listener &listener);
  void notify(const Result &result);

private:
  // ...
};
