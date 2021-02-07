#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct IDocument {
  virtual ~IDocument() = default;

  virtual std::vector<std::string> extract_text() = 0;
};

class PdfDocument : public IDocument {
 public:
  explicit PdfDocument(std::string_view path) {}
  std::vector<std::string> extract_text() override { return {"Text from PDF"}; }
};

class HtmlDocument : public IDocument {
 public:
  explicit HtmlDocument(std::string_view path) {}
  std::vector<std::string> extract_text() override {
    return {"Text from HTML"};
  }
};

class OdtDocument : public IDocument {
 public:
  explicit OdtDocument(std::string_view path) {}
  std::vector<std::string> extract_text() override { return {"Text from ODT"}; }
};

std::unique_ptr<IDocument> open(std::string_view path) {
  if (path.ends_with(".pdf"))
    return std::make_unique<PdfDocument>(path);  // C++20 addition to sv
  if (path.ends_with(".html")) return std::make_unique<HtmlDocument>(path);

  return nullptr;
}

class DocumentOpener {
 public:
  using DocumentType = std::unique_ptr<IDocument>;
  using ConcreteOpener = DocumentType (*)(std::string_view);
  //  using ConcreteOpener = std::function<DocumentType(std::string_view)>;
  //  using ConcreteOpener = tl::function_ref<DocumentType(std::string_view)>;

  void Register(std::string_view extension, ConcreteOpener opener) {
    openerByExtension.emplace(extension, opener);
  }

  DocumentType open(std::string_view path) {
    if (auto last_dot = path.find_last_of('.');
        last_dot != std::string_view::npos) {
      auto extension = path.substr(last_dot + 1);
      return openerByExtension.at(extension)(path);
    } else {
      throw std::invalid_argument{"Trying to open a file with no extension"};
    }
  }

 private:
  std::unordered_map<std::string_view, ConcreteOpener> openerByExtension;
};

int main() {
  auto document_opener = DocumentOpener{};
  document_opener.Register("pdf",
                           [](auto path) -> DocumentOpener::DocumentType {
                             return std::make_unique<PdfDocument>(path);
                           });
  document_opener.Register("html",
                           [](auto path) -> DocumentOpener::DocumentType {
                             return std::make_unique<HtmlDocument>(path);
                           });
  document_opener.Register("odt",
                           [](auto path) -> DocumentOpener::DocumentType {
                             return std::make_unique<OdtDocument>(path);
                           });

  auto document = document_opener.open("file.odt");
  std::cout << document->extract_text().front();
}
