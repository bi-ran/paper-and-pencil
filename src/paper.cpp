#include "../include/paper.h"
#include "../include/pencil.h"

#include <cmath>

void paper::add() { ++_size; }

void paper::stack(int64_t index, TObject* const object) {
    objects.push_back(object);
    indices.push_back(index);
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

void paper::accessory(std::function<void(int64_t)> a) {
    _a.push_back(a);
}

void paper::link(pencil* pencil) {
    _pencil = pencil;
}

void paper::draw(std::string const& ext) {
    using namespace std::literals::string_literals;

    if (canvas == nullptr) {
        auto description = _pencil->description();

        layout();

        canvas = new TCanvas(_tag.data(), "", 400 * _cols, 400 * _rows);
        canvas->Divide(_cols, _rows);

        for (int64_t i = 1; i <= _size; ++i) {
            canvas->cd(i);
            draw_pad(description, i);
        }
    }

    canvas->SaveAs((_tag + "."s + ext).data());
}

void paper::split(std::string const& ext) const {
    using namespace std::literals::string_literals;

    auto description = _pencil->description();

    for (int64_t i = 1; i <= _size; ++i) {
        TCanvas* c = new TCanvas(std::to_string(i).data(), "", 400, 400);

        draw_pad(description, i);

        c->SaveAs((_tag + "_p"s + std::to_string(i) + "."s + ext).data());
        c->Delete();
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
void paper::apply(TObject* const obj, std::function<void(T*)> f) const {
    if (f && obj->InheritsFrom(T::Class()))
        f(static_cast<T*>(obj));
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
    for (int64_t j = 0; j < static_cast<int64_t>(objects.size()); ++j)
        if (indices[j] == index)
            associates.push_back(objects[j]);

    return associates;
}

void paper::draw_pad(auto const& description, int64_t index) const {
    auto associates = associated(index);
    for (auto const& obj : associates) {
        apply(obj, _f);
        apply(obj, _g);
        obj->Draw("same p e");
        apply(_d);

        for (auto const& a : _a)
            apply(a, index);
    }

    draw_legend(description, associates);
}

void paper::draw_legend(auto const& description,
                        auto const& associates) const {
    auto xy = _l ? _l() : std::array<float, 4>{ 0.5, 0.9, 0.87, 0.04 };
    xy[3] = xy[2] - associates.size() * xy[3];

    TLegend* l = new TLegend(xy[0], xy[3], xy[1], xy[2]);
    apply(l, _s);

    for (auto const& obj : associates) {
        auto it = description.find(obj);
        auto desc = it != std::end(description) ? it->second
            : std::string(obj->GetName());
        l->AddEntry(obj, desc.data(), "pl");
    }

    l->Draw();
}
