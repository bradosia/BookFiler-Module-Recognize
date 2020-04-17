/*
 * @name Bookfiler™ Recognize Module
 * @author Branden Lee
 * @version 1.00
 * @license GNU LGPL v3
 * @brief text recognition.
 */

#ifndef RECOGNIZE_TEXT_INTERFACE_H
#define RECOGNIZE_TEXT_INTERFACE_H

// c++17
#include <functional>
#include <iostream>
#include <memory>
#include <string_view>
#include <vector>

/* boost 1.72.0
 * License: Boost Software License (similar to BSD and MIT)
 */
#include <boost/signals2.hpp>

/* rapidjson v1.1 (2016-8-25)
 * Developed by Tencent
 * License: MITs
 */
#include <rapidjson/document.h>

/*
 * bookfiler = BookFiler™
 */
namespace bookfiler {

#ifndef BOOKFILER_MODULE_HTTP_H
#define BOOKFILER_MODULE_HTTP_H
class Http {
public:
  void get(std::string);
  void onSocket(std::function<void(Http)>);
  void onResponse(std::function<void(Http, std::string_view)>);
  void onClose(std::function<void(Http)>);
};
#endif // end BOOKFILER_MODULE_HTTP_H

#ifndef BOOKFILER_PIXMAP_H
#define BOOKFILER_PIXMAP_H
class Pixmap {
public:
  unsigned char *data;
  // created for debugging
  unsigned int *dataUINT;
  /* @param width pixel width
   * @param height pixel height
   * @param widthBytes The number of bytes in each scan line.
   * @param pixelBytes bytes in a pixel. Usually 8, 24 or 32.
   */
  long width;
  long height;
  long widthBytes;
  long bitsPerPixel;
  long informat;
  long samplesPerPixel;
};
#endif // end BOOKFILER_PIXMAP_H

#ifndef BOOKFILER_MODULE_OCR_INTERFACE_H
#define BOOKFILER_MODULE_OCR_INTERFACE_H
class OcrMonitor {
public:
  virtual unsigned long getAvailable();
  virtual unsigned long getTotal();
};

class Ocr {
public:
  /* If the OCR API can open file
   * Try this instead
   */
  virtual bool openImageFile(std::string) = 0;
  /* @param width pixel width
   * @param height pixel height
   * @param pixelBytes bytes in a pixel. Usually 8, 24 or 32.
   */
  virtual bool openImagePixmap(unsigned char *data, long width, long height,
                               long pixelBytes) = 0;
  virtual bool openImagePixmapPtr(std::shared_ptr<Pixmap>) = 0;
  virtual std::shared_ptr<Pixmap> getPixmap() = 0;
  virtual void recognize() = 0;
  virtual void onRecognizeDone(std::function<void(std::shared_ptr<Ocr>)>) = 0;
  virtual std::shared_ptr<OcrMonitor> getRecognizeMonitor() = 0;
  virtual std::shared_ptr<OcrMonitor> getHocrMonitor() = 0;
  // Mode example: engine selection
  virtual void setMode(std::string) = 0;
  /* Type example: document type
   * general, messageInstant, messageLong, bankStatement, invoice
   */
  virtual void setType(std::string) = 0;
  virtual void setLanguage(std::vector<std::string>) = 0;
  virtual void setDataPath(std::string) = 0;
  virtual void setHttpInterface(std::shared_ptr<Http>) = 0;
  /* installing uses the HTTP interface to install data
   * from the internet
   */
  virtual void installMode(std::string) = 0;
  virtual void installType(std::string) = 0;
  virtual void installLanguage(std::vector<std::string>) = 0;
  virtual std::string getHocr() = 0;
};

class OcrInterface {
public:
  virtual void init();
  virtual void registerSettings(
      std::shared_ptr<rapidjson::Document> moduleRequest,
      std::shared_ptr<std::unordered_map<
          std::string,
          std::function<void(std::shared_ptr<rapidjson::Document>)>>>
          moduleCallbackMap);
  virtual void setSettings(std::shared_ptr<rapidjson::Value> data);
  virtual std::shared_ptr<Ocr> newOcr();
};
#endif // end BOOKFILER_MODULE_OCR_INTERFACE_H

#ifndef BOOKFILER_MODULE_PDF_H
#define BOOKFILER_MODULE_PDF_H
class PdfMonitor {
public:
  unsigned long available, total;
};

class Pdf {
public:
  void openFile(std::string);
  int getPagesTotal();
  void render(int pageNum);
  std::shared_ptr<PdfMonitor> getRenderMonitor();
  std::shared_ptr<Pixmap> getPixmap(int pageNum);
};

class PdfInterface {
public:
  virtual void init() = 0;
  virtual void registerSettings(
      std::shared_ptr<rapidjson::Document> moduleRequest,
      std::shared_ptr<std::unordered_map<
          std::string,
          std::function<void(std::shared_ptr<rapidjson::Document>)>>>
          moduleCallbackMap) = 0;
  virtual void setSettings(std::shared_ptr<rapidjson::Value> data) = 0;
  virtual std::shared_ptr<Pdf> newPdf() = 0;
};
#endif // end BOOKFILER_MODULE_PDF_H

#ifndef BOOKFILER_HOCR_WORD_H
#define BOOKFILER_HOCR_WORD_H
class HocrWord {
public:
  /* BoundingBox
   * http://kba.cloud/hocr-spec/1.2/#bbox
   */
  unsigned int x0, y0, x1, y1, w, h;
  std::string value;
  std::string id;
  // http://kba.cloud/hocr-spec/1.2/#x_wconf
  float confidence;
};
#endif // end BOOKFILER_HOCR_WORD_H

class RecognizeModel {
public:
  /* @brief Add files and directory paths to the recognizer model
   * @return vector of path handle/identifier
   * The virtual keyword stops the binary including this from trying to link
   */
  virtual void
  addPaths(std::shared_ptr<std::vector<std::string>> fileSelectedList) = 0;
  virtual void requestRecognize(std::string fileRequested) = 0;
  boost::signals2::signal<void(std::shared_ptr<Pixmap>)> imageUpdateSignal;
  boost::signals2::signal<void(
      std::shared_ptr<std::vector<std::shared_ptr<HocrWord>>>)>
      textUpdateSignal;
};

/* RecognizeInterface
 * Methods must be virtual or the linker compains
 */

class RecognizeInterface {
public:
  virtual void init() = 0;
  virtual void registerSettings(
      std::shared_ptr<rapidjson::Document> moduleRequest,
      std::shared_ptr<std::unordered_map<
          std::string,
          std::function<void(std::shared_ptr<rapidjson::Document>)>>>
          moduleCallbackMap) = 0;
  virtual void setSettings(std::shared_ptr<rapidjson::Value> data) = 0;
  virtual std::shared_ptr<RecognizeModel> newModel() = 0;
  virtual void setPdfModule(std::shared_ptr<PdfInterface>) = 0;
  virtual void setOcrModule(std::shared_ptr<OcrInterface>) = 0;
};

} // namespace bookfiler

#endif
// end RECOGNIZE_TEXT_INTERFACE_H
