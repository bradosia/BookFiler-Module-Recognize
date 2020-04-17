/*
 * @name Bookfiler™ Recognize Module
 * @author Branden Lee
 * @version 1.00
 * @license GNU LGPL v3
 * @brief text recognition.
 */

#ifndef BOOKFILER_MODULE_RECOGNIZE_EXPORT_H
#define BOOKFILER_MODULE_RECOGNIZE_EXPORT_H

// Config
#include "core/config.hpp"

// C++17
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT

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
#include "Interface.hpp"
#include "core/recognizeModel.hpp"

/*
 * bookfiler = BookFiler™
 */
namespace bookfiler {

/*
 * This widget uses the MVC design pattern.
 * This is the controller, the view is a QT widget, and the model is the API
 * storage
 */
class ModuleExport : public RecognizeInterface {
  std::vector<std::shared_ptr<RecognizeModelInternal>> modelList;
  std::shared_ptr<OcrInterface> ocrModule;
  std::shared_ptr<PdfInterface> pdfModule;

public:
  ModuleExport();
  ~ModuleExport();

  void init();
  void registerSettings(
      std::shared_ptr<rapidjson::Document> moduleRequest,
      std::shared_ptr<std::unordered_map<
          std::string,
          std::function<void(std::shared_ptr<rapidjson::Document>)>>>
          moduleCallbackMap);
  void setSettings(std::shared_ptr<rapidjson::Value> data);
  std::shared_ptr<RecognizeModel> newModel();
  void setPdfModule(std::shared_ptr<PdfInterface>);
  void setOcrModule(std::shared_ptr<OcrInterface>);
};

} // namespace bookfiler

#endif
// end BOOKFILER_MODULE_RECOGNIZE_EXPORT_H
