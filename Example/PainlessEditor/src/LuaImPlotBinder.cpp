#include "LuaImPlotBinder.h"
#include "CoreRender/Renderer/Colors.h"
#include "CustomPanel.h"
#include "imgui.h"
#include "implot.h"

#define ImDrawIdx unsigned int
namespace painless
{
// Dumb lua vector wrapper
template <typename T> struct LuaVector {
  std::vector<T> data;
  LuaVector() = default;
  LuaVector(size_t n) : data() { data.reserve(n); }
  size_t size() const
  {
    LUA_LOG_I(data.size());
    return data.size();
  }
  bool empty() const { return data.empty(); }
  void clear() { data.clear(); }
  void push_back(const T &v) { data.push_back(v); }
  T &get(size_t i)
  {
    if (i == 0 || i > data.size())
      throw std::out_of_range("index");
    return data[i - 1];
  }
  void set(size_t i, const T &v)
  {
    if (i == data.size() + 1)
      return data.push_back(v);
    if (i == 0 || i > data.size())
      throw std::out_of_range("index == 0 or index > vector size");
    data[i - 1] = v;
  }
  const T *raw() const { return data.data(); }
  // -----------------------------------------------------------
  // Dumb Utilities
  // -----------------------------------------------------------
  T max() { return *std::max_element(data.begin(), data.end()); }
  T min() { return *std::min_element(data.begin(), data.end()); }

  // TODO: any compression loses data, so this could be done exclusevily on
  // implot. But right now, no need
  LuaVector<T> downsample(int finalSize_ = 1) const
  {
    uint32_t finalSize = static_cast<uint32_t>(finalSize_);
    if (finalSize > data.size() || finalSize <= 0)
      return *this;
    LuaVector<T> result;
    result.data.reserve(finalSize);
    size_t step = data.size() / finalSize;
    if (step == 0)
      step = 1;
    for (size_t i = 0; i < data.size(); i += step)
      result.data.push_back(data[i]);
    return result;
  }
  // -----------------------------------------------------------
  // DEBUG
  // -----------------------------------------------------------
  void print(size_t head) const
  {
    // NOTE: this assumes T is simple primary data like double, int, float
    std::ostringstream out;
    size_t s = std::min(data.size(), head);
    out << "";
    for (size_t i = 0; i < s; i++) {
      out << data[i] << ", ";
    }
    LUA_LOG_I(out.str());
  }
  void print_tail(size_t tail) const

  {
    // NOTE: this assumes T is simple primary data like double, int, float
    std::ostringstream out;
    size_t s = std::min(data.size(), tail);
    out << "";
    for (size_t i = data.size() - 1; data.size() - 1 - i < s; i--) {
      out << data[i] << ", ";
    }
    LUA_LOG_I(out.str());
  }
  void printCoordinates(const LuaVector<T> &o) const
  {
    size_t size = std::min(o.data.size(), data.size());
    std::ostringstream out;
    for (size_t i = 0; i < size; i++) {
      out << "(" << data[i] << ", " << o.data[i] << "), ";
    }
    LUA_LOG_I("sizes = ({},{})", o.data.size(), data.size());
    LUA_LOG_I(out.str());
  }
};
// "ipairs",
//       [&sol](LuaVector<T> &v) {
//         size_t i = 0;
//
//         return [&, i]() mutable -> std::tuple<size_t, sol::object> {
//           ++i;
//
//           if (i > v.size())
//             return {0, sol::nil};
//           return {i, sol::make_object(sol, v.get(i))};
//         };
//       },
template <typename T> void bindVector(sol::state &sol, const std::string &name)
{
  sol.new_usertype<LuaVector<T>>(
      name,
      sol::constructors<                                    //
          LuaVector<T>(),                                   //
          LuaVector<T>(size_t)>(),                          //
      "size", &LuaVector<T>::size,                          //
      "empty", &LuaVector<T>::empty,                        //
      "clear", &LuaVector<T>::clear,                        //
      "push_back", &LuaVector<T>::push_back,                //
      "downsample", &LuaVector<T>::downsample,              //
      "print", &LuaVector<T>::print,                        //
      "print_tail", &LuaVector<T>::print_tail,              //
      "max", &LuaVector<T>::max,                            //
      "min", &LuaVector<T>::min,                            //
      "print_coordinates", &LuaVector<T>::printCoordinates, //
      "ipairs",
      [](LuaVector<T> &v) {
        return sol::as_function([&v](int i) -> std::tuple<int, T *> {
          ++i;
          if (i > static_cast<int>(v.size()))
            return {0, nullptr};
          return {i, &v.get(i)};
        });
      },
      sol::meta_function::index,
      [](LuaVector<T> &v, int i) -> T & { return v.get(i); },
      sol::meta_function::new_index,
      [](LuaVector<T> &v, size_t i, const T &value) { v.set(i, value); },
      sol::meta_function::length, &LuaVector<T>::size //
  );
}
// -------------------------------------------------------------
// Constant Enums
// -------------------------------------------------------------

void luabinder::unbindImPlot(sol::state &lua)
{
  lua["ImPlot"] = sol::nil;
  lua["ImAxis"] = sol::nil;
  lua["ImPlotScale"] = sol::nil;
  lua["ImPlotCond"] = sol::nil;
}

void luabinder::bindImPlot(sol::state &lua)
{
  lua.new_enum(       //
      "ImAxis",       //
      "X", ImAxis_X1, //
      "Y", ImAxis_Y1  //
  );
  lua.new_enum(                                   //
      "ImPlotScale",                              //
      "Linear", ImPlotScale_::ImPlotScale_Linear, //
      "Time", ImPlotScale_::ImPlotScale_Time,     //
      "Log10", ImPlotScale_::ImPlotScale_Log10,   //
      "SymLog", ImPlotScale_::ImPlotScale_SymLog  //
  );
  customPanel::getImPlotTable() = lua.create_named_table("ImPlot");
  customPanel::getImPlotTable().set_function(
      "BeginPlot", [](const std::string &title, float width, float height) {
        return ImPlot::BeginPlot(title.c_str(), ImVec2(width, height));
      });
  customPanel::getImPlotTable().set_function("EndPlot",
                                             []() { ImPlot::EndPlot(); });
  lua.new_enum(                    //
      "ImPlotCond",                //
      "Always", ImPlotCond_Always, //
      "Once", ImPlotCond_Once,     //
      "None", ImPlotCond_None      //
  );
  customPanel::getImPlotTable().set_function(
      "SetupAxes", [](const std::string &x, const std::string &y) {
        ImPlot::SetupAxes(x.c_str(), y.c_str());
      });

  customPanel::getImPlotTable().set_function(
      "SetupAxesLimits", [](double x_min, double x_max, double y_min,
                            double y_max, sol::optional<ImPlotCond> c) {
        ImPlot::SetupAxesLimits(x_min, x_max, y_min, y_max,
                                c.value_or(ImPlotCond_None));
      });
  customPanel::getImPlotTable().set_function(
      "SetNextAxesLimits", [](double x_min, double x_max, double y_min,
                              double y_max, sol::optional<ImPlotCond> c) {
        ImPlot::SetNextAxesLimits(x_min, x_max, y_min, y_max,
                                  c.value_or(ImPlotCond_None));
      });
  customPanel::getImPlotTable().set_function(
      "PushStyleColor", [](pain::Color c) {
        ImPlot::PushStyleColor(ImPlotCol_InlayText, c.value);
      });
  customPanel::getImPlotTable().set_function("PopStyleColor",
                                             []() { ImPlot::PopStyleColor(); });

  customPanel::getImPlotTable().set_function(
      "PlotLine",
      // sol::overload(
      [](const std::string &label, const LuaVector<double> &xs,
         const LuaVector<double> &ys, sol::optional<pain::Color> c) {
        size_t count = std::min(xs.data.size(), ys.data.size());
        ImPlotSpec spec;
        spec.Flags = ImPlotAxisFlags_RangeFit;
        if (c)
          spec.LineColor = ImGui::ColorConvertU32ToFloat4(c.value().value);
        ImPlot::PlotLine(label.c_str(), xs.data.data(), ys.data.data(),
                         static_cast<int>(count), spec);
      } //
        // [](const std::string &label, sol::table xs_table,
        //    sol::table ys_table) {
        //   size_t count = std::min(xs_table.size(), ys_table.size());
        //
        //   std::vector<double> xs(count);
        //   std::vector<double> ys(count);
        //
        //   for (size_t i = 1; i <= count; ++i) {
        //     xs[i - 1] = xs_table.get<double>(i);
        //     ys[i - 1] = ys_table.get<double>(i);
        //   }
        //
        //   ImPlot::PlotLine(label.c_str(), xs.data(), ys.data(),
        //   (int)count);
        // } //
        // )
  );
  customPanel::getImPlotTable().set_function( //
      "SetupAxesScale",                       //
      [](const ImAxis &a, const ImPlotScale_ &s) {
        ImPlot::SetupAxisScale(a, s);
      });
  customPanel::getImPlotTable().set_function(
      "PlotScatter",
      sol::overload(
          [](const std::string &label, const LuaVector<double> &xs,
             const LuaVector<double> &ys) {
            size_t count = std::min(xs.data.size(), ys.data.size());
            ImPlotSpec spec;
            spec.Flags = ImPlotAxisFlags_RangeFit;
            ImPlot::PlotScatter(label.c_str(), xs.data.data(), ys.data.data(),
                                static_cast<int>(count), spec);
          }, //
          [](const std::string &label, const sol::table &xs_table,
             const sol::table &ys_table) {
            size_t count = std::min(xs_table.size(), ys_table.size());

            std::vector<double> xs(count);
            std::vector<double> ys(count);

            for (size_t i = 1; i <= count; ++i) {
              xs[i - 1] = xs_table.get<double>(i);
              ys[i - 1] = ys_table.get<double>(i);
            }

            ImPlot::PlotScatter(label.c_str(), xs.data(), ys.data(),
                                (int)count);
          } //
          ));

  bindVector<double>(lua, "VecDouble");
  bindVector<float>(lua, "VecFloat");
  bindVector<int>(lua, "VecInt");
}

} // namespace painless
