/*
 * @name Bookfiler™ Recognize Module
 * @author Branden Lee
 * @version 1.00
 * @license GNU LGPL v3
 * @brief text recognition.
 */

// Local Project
#include "recognizeModel.hpp"

/*
 * bookfiler = BookFiler™
 */
namespace bookfiler {

RecognizeModelInternal::RecognizeModelInternal(
    std::shared_ptr<OcrInterface> ocrModule_,
    std::shared_ptr<PdfInterface> pdfModule_)
    : ocrModule(ocrModule_), pdfModule(pdfModule_) {}
RecognizeModelInternal::~RecognizeModelInternal() {}

void RecognizeModelInternal::addPaths(
    std::shared_ptr<std::vector<std::string>> fileSelectedList) {
#if BOOKFILER_RECOGNIZE_MODEL_ADD_PATHS
  std::cout << "bookfiler::RecognizeModel::addPaths:\n";
  for (auto a : *fileSelectedList) {
    std::cout << a << "\n";
  }
#endif
}

void RecognizeModelInternal::requestRecognize(std::string fileRequested) {
#if BOOKFILER_RECOGNIZE_MODEL_REQUEST_RECOGNIZE
  std::cout << "bookfiler::RecognizeModelInternal::requestRecognize("
            << fileRequested << ")\n";
#endif
  if (!ocrModule) {
#if BOOKFILER_RECOGNIZE_MODEL_REQUEST_RECOGNIZE
    std::cout << "bookfiler::RecognizeModelInternal::requestRecognize("
              << fileRequested << ") ERROR: ocrModule is null\n";
#endif
    return;
  }
  std::shared_ptr<Ocr> ocrFile = ocrModule->newOcr();
  ocrFile->setMode("");
  ocrFile->setType("");
  ocrFile->setLanguage({"eng"});
  ocrFile->setDataPath("");
  // call Init before attempting to set an image
  ocrFile->openImageFile(fileRequested);
  imageUpdateSignal(ocrFile->getPixmap());
  ocrFile->onRecognizeDone(std::bind(&RecognizeModelInternal::recognizeDone,
                                     this, std::placeholders::_1));
  ocrFile->recognize();
}

void RecognizeModelInternal::recognizeDone(std::shared_ptr<Ocr> ocrPtr) {
  /* IMPORANT!
   * wrapping ptree as a shared_ptr causes many bugs:
   * std::shared_ptr<boost::property_tree::ptree>
   * Don't do this in the future
   */
  boost::property_tree::ptree hocrTree;
  std::string data(ocrPtr->getHocr());
#if BOOKFILER_RECOGNIZE_MODEL_RECOGNIZE_DONE_DEBUG
  std::cout << "bookfiler::RecognizeModelInternal::recognizeDone:\n"
            << data << "\n";
#endif
  // stream prevents data being recopied
  boost::iostreams::stream<boost::iostreams::array_source> stream(data.c_str(),
                                                                  data.size());
  boost::property_tree::read_xml(stream, hocrTree);
#if BOOKFILER_RECOGNIZE_MODEL_RECOGNIZE_DONE_DEBUG
  std::cout << "bookfiler::RecognizeModelInternal::recognizeDone:\n";
  printPropertyTree(hocrTree);
#endif
  toBankStatementTable(hocrTree);
}

void RecognizeModelInternal::printPropertyTree(
    boost::property_tree::ptree &tree) {
  /* Depth First Traversal uses a stack. BFT uses a queue.
   */
  std::stack<std::shared_ptr<propertyTreePrint>> stack;
  try {
    std::shared_ptr<propertyTreePrint> rootRow =
        std::make_shared<propertyTreePrint>(0, "root", tree);
    stack.push(rootRow);
  } catch (...) {
    std::cout
        << "bookfiler::RecognizeModelInternal::printPropertyTree EXCEPTION\n";
  }
  while (!stack.empty()) {
    std::shared_ptr<propertyTreePrint> subTree = stack.top();
    stack.pop();
    std::cout << std::string(subTree->depth, ' ') << subTree->depth << ":"
              << subTree->name << ":"
              << boost::algorithm::trim_copy(subTree->node.data()) << "\n";
    // get children
    boost::optional<boost::property_tree::ptree &> treeBranchOpt =
        subTree->node.get_child_optional("");
    if (treeBranchOpt.is_initialized()) {
      for (auto element : treeBranchOpt.value()) {
        try {
          stack.push(std::make_shared<propertyTreePrint>(
              subTree->depth + 1, element.first, element.second));
        } catch (...) {
          std::cout << std::string(subTree->depth, ' ') << subTree->depth << ":"
                    << subTree->name << ":"
                    << boost::algorithm::trim_copy(subTree->node.data())
                    << "<-EXCEPTION\n";
        }
      }
    }
  }
}

void RecognizeModelInternal::toBankStatementTable(
    boost::property_tree::ptree hocrTree) {
#if BOOKFILER_RECOGNIZE_MODEL_TO_STATEMENT_TABLE_DEBUG2
  std::cout << "bookfiler::RecognizeModelInternal::toBankStatementTable:\n";
  printPropertyTree(hocrTree);
#endif
  std::vector<boost::property_tree::ptree> nodeList;
  std::shared_ptr<FileTypeBankStatement> bankStatement =
      std::make_shared<FileTypeBankStatement>();
  /* Depth First Traversal
   */
  std::stack<std::shared_ptr<HocrWordCandidate>> stack;
  std::shared_ptr<HocrWordCandidate> rootRow =
      std::make_shared<HocrWordCandidate>(0, "root", "", hocrTree);
  stack.push(rootRow);
  while (!stack.empty()) {
    std::shared_ptr<HocrWordCandidate> nextCandidate = stack.top();
    stack.pop();
#if BOOKFILER_RECOGNIZE_MODEL_TO_STATEMENT_TABLE_DEBUG
    std::cout << std::string(nextCandidate->depth, ' ') << nextCandidate->depth
              << ":" << nextCandidate->name << ":"
              << boost::algorithm::trim_copy(nextCandidate->node.data())
              << "\n";
#endif
    // check if word
    if (nextCandidate->name == "class" &&
        nextCandidate->node.data() == "ocrx_word" &&
        nextCandidate->parent1.size() > 0 &&
        nextCandidate->parentName1 == "<xmlattr>" &&
        nextCandidate->parent2.size() > 0) {
      nodeList.push_back(nextCandidate->parent2);
    }
    // get children
    boost::optional<boost::property_tree::ptree &> treeBranchOpt =
        nextCandidate->node.get_child_optional("");
    if (treeBranchOpt.is_initialized()) {
      for (auto element : treeBranchOpt.value()) {
        try {
          stack.push(std::make_shared<HocrWordCandidate>(
              nextCandidate->depth + 1, element.first, nextCandidate->name,
              element.second, nextCandidate->node, nextCandidate->parent1));
        } catch (...) {
          std::cout << std::string(nextCandidate->depth, ' ')
                    << nextCandidate->depth << ":" << nextCandidate->name << ":"
                    << boost::algorithm::trim_copy(nextCandidate->node.data())
                    << "<-EXCEPTION\n";
        }
      }
    }
  }
#if BOOKFILER_RECOGNIZE_MODEL_TO_STATEMENT_TABLE_DEBUG2
  for (auto node : nodeList) {
    std::cout
        << "bookfiler::RecognizeModelInternal::toBankStatementTable NODE:\n";
    printPropertyTree(node);
  }
#endif
  /* Convert boost::property_tree::ptree to HocrWord
   */
  std::shared_ptr<std::vector<std::shared_ptr<HocrWord>>> wordList =
      std::make_shared<std::vector<std::shared_ptr<HocrWord>>>();
  for (auto node : nodeList) {
    std::shared_ptr<HocrWord> wordPtr = std::make_shared<HocrWord>();
    wordPtr->value = node.data();
    boost::optional<boost::property_tree::ptree &> treeBranchOpt =
        node.get_child_optional("<xmlattr>");
    if (treeBranchOpt.is_initialized()) {
      for (auto element : treeBranchOpt.value()) {
        if (element.first == "title") {
          std::vector<std::string> attributeProperties;
          boost::split(attributeProperties, element.second.data(),
                       boost::is_any_of(";"));
          for (std::string property : attributeProperties) {
            std::vector<std::string> propertyParts;
            /* Linux - Ubuntu
             * error: cannot bind non-const lvalue reference of
             * type ‘std::__cxx11::basic_string<char>&’ to an rvalue of type
             * ‘std::__cxx11::basic_string<char>’ Fix: first assign to temporary
             * string propertyTrim
             */
            std::string propertyTrim = boost::algorithm::trim_copy(property);
            boost::split(propertyParts, propertyTrim, boost::is_any_of(" "));
            /* Get Attributes
             */
            try {
              if (propertyParts.size() == 5 && propertyParts.at(0) == "bbox") {
                wordPtr->x0 = std::stoi(propertyParts.at(1));
                wordPtr->y0 = std::stoi(propertyParts.at(2));
                wordPtr->x1 = std::stoi(propertyParts.at(3));
                wordPtr->y1 = std::stoi(propertyParts.at(4));
                wordPtr->w = wordPtr->x1 - wordPtr->x0;
                wordPtr->h = wordPtr->y1 - wordPtr->y0;
              } else if (propertyParts.size() == 2 &&
                         propertyParts.at(0) == "x_wconf") {
                wordPtr->confidence = std::stof(propertyParts.at(1));
              }
            } catch (...) {
            }
          }
        } else if (element.first == "id") {
          wordPtr->id = element.second.data();
        }
      }
    }
    wordList->push_back(wordPtr);
  }

#if BOOKFILER_RECOGNIZE_MODEL_TO_STATEMENT_TABLE_DEBUG
  for (auto wordPtr : *wordList) {
    std::cout << "x0=" << wordPtr->x0 << " y0=" << wordPtr->y0
              << " x1=" << wordPtr->x1 << " y1=" << wordPtr->y1 << "\n";
  }
#endif
  textUpdateSignal(wordList);
}

} // namespace bookfiler
