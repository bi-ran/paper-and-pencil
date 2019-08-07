#include "../include/paper.h"
#include "../include/pencil.h"

#include <cmath>

using namespace std::literals::string_literals;

void paper::add() { ++_size; }

void paper::add(int64_t size) { _size += size; }

void paper::stack(int64_t index, TObject* const object) {
    order.push_back(object);
    objects[object] = index;
}

void paper::add(TObject* const object) {
    add();
    stack(_size, object);
}

void paper::stack(TObject* const object) {
    stack(_size, object);
}

void paper::divide(int64_t cols, int64_t rows) {
    _cols = cols;
    _rows = rows;
}

void paper::adjust(TObject* const object, std::string const& dopt,
                   std::string const& lopt) {
    if (!dopt.empty()) { dopts[object] = dopt; }
    if (!lopt.empty()) { lopts[object] = lopt; }
}

void paper::accessory(std::function<void(int64_t)> a) {
    _a.push_back(a);
}

void paper::jewellery(std::function<void(TH1*, int64_t)> j) {
    _j.push_back(j);
}

void paper::link(pencil* pencil) {
    _pencil = pencil;
}

void paper::draw(std::string const& ext) {
    if (canvas == nullptr) {
        auto description = _pencil ? _pencil->description()
            : std::map<TObject* const, std::string>();

        layout();

        canvas = new TCanvas(_tag.data(), "", 400 * _cols, 400 * _rows);
        canvas->Divide(_cols, _rows, 0.01 / _cols, 0.01 / _rows);

        for (int64_t i = 1; i <= _size; ++i) {
            canvas->cd(i);
            auto associates = associated(i);
            draw_pad(associates, i);
            draw_legend(associates, description);
        }
    }

    canvas->SaveAs((_tag + "."s + ext).data());
}

void paper::split(std::string const& ext) const {
    auto description = _pencil ? _pencil->description()
        : std::map<TObject* const, std::string>();

    for (int64_t i = 1; i <= _size; ++i) {
        TCanvas* c = new TCanvas(std::to_string(i).data(), "", 400, 400);

        auto associates = associated(i);
        draw_pad(associates, i);
        draw_legend(associates, description);

        c->SaveAs((_tag + "_p"s + std::to_string(i) + "."s + ext).data());
        delete c;
    }
}

template <typename T>
void paper::apply(std::function<T> f) const {
    if (f) { f(); }
}

template <typename T>
void paper::apply(std::function<T> f, int64_t index) const {
    if (f) { f(index); }
}

template <typename T>
void paper::apply(std::function<void(T*)> f, TObject* const obj) const {
    if (f && obj->InheritsFrom(T::Class()))
        f(static_cast<T*>(obj));
}

template <typename T>
void paper::apply(std::function<void(T*, int64_t)> f, TObject* const obj,
                  int64_t index) const {
    if (f && obj->InheritsFrom(T::Class()))
        f(static_cast<T*>(obj), index);
}

void paper::layout() {
    if (!_cols || !_rows) {
        float rows = std::ceil(std::sqrt(_size));
        float cols = std::ceil(_size / rows);

        _cols = cols;
        _rows = rows;
    } else if (_rows < 0) {
        _rows = std::ceil(_size / _cols);
    } else if (_cols < 0) {
        _cols = std::ceil(_size / _rows);
    }
}

std::vector<TObject*> paper::associated(int64_t index) const {
    std::vector<TObject*> associates;
    for (auto const& obj : order)
        if (objects.find(obj)->second == index)
            associates.push_back(obj);

    return associates;
}

void paper::draw_pad(auto const& associates, int64_t index) const {
    for (auto const& obj : associates) {
        apply(_f, obj);
        apply(_g, obj);

        auto it = dopts.find(obj);
        auto opt = (it != dopts.end()) ? it->second : "pe"s;
        obj->Draw(("same "s + opt).data());

        apply(_d);

        for (auto const& a : _a)
            apply(a, index);
        for (auto const& j : _j)
            apply(j, obj, index);
    }

    if (_flags & flags::logx) { gPad->SetLogx(); }
    if (_flags & flags::logy) { gPad->SetLogy(); }
}

void paper::draw_legend(auto const& associates,
                        auto const& description) const {
    if (_flags & flags::key) { return; }

    int64_t count = 0;
    for (auto const& obj : associates)
        if (description.find(obj) != std::end(description))
            ++count;

    auto xy = _l ? _l() : std::array<float, 4>{ 0.5, 0.9, 0.87, 0.04 };
    xy[3] = xy[2] - count * xy[3];

    TLegend* l = new TLegend(xy[0], xy[3], xy[1], xy[2]);
    apply(_s, l);

    for (auto const& obj : associates) {
        auto itd = description.find(obj);
        if (itd == std::end(description)) { continue; }
        auto desc = itd->second;

        auto itl = lopts.find(obj);
        auto opt = (itl != lopts.end()) ? itl->second : "pl"s;
        l->AddEntry(obj, desc.data(), opt.data());
    }

    l->Draw();
}
