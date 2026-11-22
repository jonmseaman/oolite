// What an RmlUi view is given by the host that made it: the context to load
// its document into, where documents live, and a place in the list of views
// the host draws and asks after each update.

#pragma once

#include <filesystem>
#include <string_view>

namespace Rml {
class Context;
}  // namespace Rml

namespace oo::ui {

class RmlDocumentView {
 public:
  virtual ~RmlDocumentView() = default;

  // Its document is on screen (and replaces the text grid).
  virtual bool IsShowing() const = 0;
  // The context has just laid out: element positions are current.
  virtual void OnContextUpdated() = 0;
};

class RmlDocumentHost {
 public:
  virtual ~RmlDocumentHost() = default;

  virtual Rml::Context& context() = 0;
  // Resources/UI/<file_name>.
  virtual std::filesystem::path DocumentPath(
      std::string_view file_name) const = 0;

  // A view registers once its document is loaded and unregisters before it is
  // destroyed.
  virtual void Register(RmlDocumentView* view) = 0;
  virtual void Unregister(RmlDocumentView* view) = 0;
};

}  // namespace oo::ui
