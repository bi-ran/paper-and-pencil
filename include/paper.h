#ifndef PAPER_H
#define PAPER_H

#include <array>
#include <functional>
#include <map>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

#include "TCanvas.h"
#include "TObject.h"
#include "TGraph.h"
#include "TH1.h"
#include "TLegend.h"

#include "pencil.h"

class paper {
  public:
    paper(std::string const& tag, pencil* p, int64_t cols, int64_t rows)
        : _tag(tag),
          _size(0),
          _cols(cols),
          _rows(rows),
          _pencil(p),
          canvas(nullptr) { }

    paper(std::string const& tag)
        : paper(tag, nullptr, 0, 0) { }

    paper(std::string const& tag, pencil* p)
        : paper(tag, p, 0, 0) { }

    paper(std::string const& tag, int64_t cols, int64_t rows)
        : paper(tag, nullptr, cols, rows) { }

    paper(paper const&) = delete;
    paper& operator=(paper const&) = delete;
    ~paper() = default;

    enum flags { key = 1 << 0 };

    void add();

    void stack(int64_t index, TObject* const object);

    template <typename... T>
    void stack(int64_t index, TObject* const object, T const&... adjectives) {
        stack(index, object); _pencil->describe(object, adjectives...); }

    void add(TObject* const object);

    template <typename... T>
    void add(TObject* const object, T const&... adjectives) {
        add(object); _pencil->describe(object, adjectives...); }

    void stack(TObject* const object);

    template <typename... T>
    void stack(TObject* const object, T const&... adjectives) {
        stack(object); _pencil->describe(object, adjectives...); }

    void adjust(TObject* const object, std::string const& dopt,
                std::string const& lopt);

    void divide(int64_t cols, int64_t rows);

    template <typename... T>
    void set(T... options) {
        for (auto opt : { options... }) { _flags = _flags ^ opt; }
    }

    void decorate(std::function<void()> d)                { _d = d; }
    void format(std::function<void(TH1*)> f)              { _f = f; }
    void format(std::function<void(TGraph*)> g)           { _g = g; }
    void legend(std::function<std::array<float, 4>()> l)  { _l = l; }
    void style(std::function<void(TLegend*)> s)           { _s = s; }

    void accessory(std::function<void(int64_t)> a);

    void link(pencil* pencil);

    void draw(std::string const& ext);

    void split(std::string const& ext) const;

  private:
    template <typename T>
    void apply(TObject* const obj, std::function<void(T*)> f) const;

    template <typename T>
    void apply(std::function<T> f) const;

    template <typename T>
    void apply(std::function<T> f, int64_t index) const;

    void layout();

    std::vector<TObject*> associated(int64_t index) const;

    void draw_pad(auto const& associates, int64_t index) const;

    void draw_legend(auto const& associates, auto const& description) const;

    std::string const _tag;
    int64_t _size;
    int64_t _cols;
    int64_t _rows;

    uint64_t _flags = 0;

    std::map<TObject*, std::string> objects;

    std::unordered_map<TObject*, std::string> dopts;
    std::unordered_map<TObject*, std::string> lopts;

    std::function<void()> _d;
    std::function<void(TH1*)> _f;
    std::function<void(TGraph*)> _g;
    std::function<std::array<float, 4>()> _l;
    std::function<void(TLegend*)> _s;

    std::vector<std::function<void(int64_t)>> _a;

    pencil* _pencil;

    TCanvas* canvas;
};

#endif /* PAPER_H */
