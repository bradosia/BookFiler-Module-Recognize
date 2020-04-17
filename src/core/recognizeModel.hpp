/*
 * @name Bookfiler™ Recognize Module
 * @author Branden Lee
 * @version 1.00
 * @license GNU LGPL v3
 * @brief text recognition.
 */

#ifndef BOOKFILER_MODULE_RECOGNIZE_MODEL_H
#define BOOKFILER_MODULE_RECOGNIZE_MODEL_H

// config
#include "config.hpp"

// c++17
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <stack>
#include <string>
#include <unordered_map>
#include <vector>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/algorithm/string.hpp>
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include <boost/iostreams/stream.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/reader.h> // rapidjson::ParseResult
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

// Local Project
#include "../Interface.hpp"

/*
 * bookfiler = BookFiler™
 */
namespace bookfiler {

class propertyTreePrint {
public:
  propertyTreePrint(unsigned int depth_, std::string name_,
                    boost::property_tree::ptree node_)
      : depth(depth_), name(name_), node(node_){};
  unsigned int depth;
  std::string name;
  boost::property_tree::ptree node;
};

/* Used for Hocr tree traversal
 * The word node is two parents up from a node with class=ocrx_word
 */
class HocrWordCandidate {
public:
  HocrWordCandidate(
      unsigned int depth_, std::string name_, std::string parentName1_,
      boost::property_tree::ptree node_,
      boost::property_tree::ptree parent1_ = boost::property_tree::ptree(),
      boost::property_tree::ptree parent2_ = boost::property_tree::ptree())
      : depth(depth_), name(name_), parentName1(parentName1_), node(node_),
        parent1(parent1_), parent2(parent2_){};
  unsigned int depth;
  std::string name, parentName1;
  boost::property_tree::ptree node, parent1, parent2;
};

class FileTypeBankStatementRow {
public:
  bool amountSign, balanceSign;
  unsigned long long amount, balance;
  std::string date, description;
};

class FileTypeBankStatement {
public:
  std::unordered_map<unsigned int, std::shared_ptr<HocrWord>> hocrWordMap;
  std::unordered_map<unsigned int, FileTypeBankStatementRow> rowMap;
};

class RecognizeFile {
public:
  std::unordered_map<unsigned int, std::shared_ptr<Ocr>> ocrMap;
  std::unordered_map<unsigned int, std::shared_ptr<boost::property_tree::ptree>>
      hocrMap;
};

class RecognizeModelInternal : public RecognizeModel {
private:
  std::unordered_map<std::string, std::shared_ptr<RecognizeFile>>
      recognizeFileMap;
  std::shared_ptr<OcrInterface> ocrModule;
  std::shared_ptr<PdfInterface> pdfModule;

public:
  RecognizeModelInternal(std::shared_ptr<OcrInterface> ocrModule_,
                         std::shared_ptr<PdfInterface> pdfModule_);
  ~RecognizeModelInternal();
  void addPaths(std::shared_ptr<std::vector<std::string>> fileSelectedList);
  void requestRecognize(std::string fileRequested);
  void recognizeDone(std::shared_ptr<Ocr>);
  void printPropertyTree(boost::property_tree::ptree &tree);
  // for the Bookfiler™ Accounting
  void toBankStatementTable(boost::property_tree::ptree hocrTree);
};

} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_RECOGNIZE_MODEL_H
