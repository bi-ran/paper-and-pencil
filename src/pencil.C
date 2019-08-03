#include "../include/pencil.h"

#include <iterator>
#include <numeric>

#include "TColor.h"
#include "TGraph.h"

using namespace std::literals::string_literals;

static std::vector<int32_t> const colours = {
    TColor::GetColor("#515151"),
    TColor::GetColor("#f2777a"),
    TColor::GetColor("#f99157"),
    TColor::GetColor("#ffcc66"),
    TColor::GetColor("#99cc99"),
    TColor::GetColor("#6699cc"),
    TColor::GetColor("#9999cc"),
};

static std::vector<int32_t> const msolid = { 20, 21, 22, 23, 29, 41 };
static std::vector<int32_t> const mopen  = { 24, 25, 26, 32, 30, 40 };

void pencil::set_binary(std::string const& label) {
    binary = categories[label][0];
}

template <typename T, template <typename...> class U>
void pencil::operator()(T* const obj, U<int64_t> const& attrs) const {
    std::vector<int64_t> indices(std::begin(attrs), std::end(attrs));

    for (auto const& pair : dittoes) {
        auto iorig = attributes.find(pair.first);
        auto iattr = attributes.find(pair.second);

        if (iorig != std::end(attributes) && iattr != std::end(attributes)) {
            auto orig = iorig->second;
            auto attr = iattr->second;
            if (orig[0] == attr[0] && attrs[orig[0]] == orig[1])
                indices[attr[0]] = attr[1];
        }
    }

    for (auto const& pair : styles) {
        auto const& attr = attributes.find(pair.first)->second;
        if (indices[attr[0]] == attr[1]) {
            auto style = styles.find(pair.first)->second;
            if (obj->InheritsFrom(TH1::Class())) {
                style(static_cast<TH1*>(obj));
                return;
            }
        }
    }

    int64_t colour_index = 0;
    int64_t marker_index = 0;
    int64_t marker_type = 0;

    for (int64_t i = 0; i < static_cast<int64_t>(features.size()); ++i) {
        switch (features[i]) {
            case -1: marker_type = indices[i]; break;
            case 0: colour_index = indices[i]; break;
            case 1: marker_index = indices[i]; break;
        }
    }

    int32_t colour = colours[colour_index];
    int32_t marker = (*(marker_type ? &mopen : &msolid))[marker_index];

    apply<TH1>(obj, colour, marker);
    apply<TGraph>(obj, colour, marker);
}

template <typename T, template <typename...> class U>
void pencil::operator()(T* const obj, U<std::string> const& adjectives) const {
    std::vector<int64_t> attrs(adjectives.size());
    for (auto const& adj : adjectives) {
        auto attr = attributes[adj];
        attrs[attr[0]] = attr[1];
    }

    (*this)(obj, attrs);
}

void pencil::sketch() {
    set_features(categories.size());
    for (auto const& obj : objects)
        (*this)(obj.first, obj.second);
}

void pencil::alias(std::string const& label, std::string const& formal) {
    aliases[label] = formal;
}

void pencil::ditto(std::string const& label, std::string const& target) {
    dittoes[label] = target;
}

void pencil::style(std::string const& label, std::function<void(TH1*)> style) {
    styles[label] = style;
}

std::map<TObject* const, std::string> pencil::description() const {
    std::map<TObject* const, std::string> desc;

    /* build reverse map (intended to be called only once!) */
    std::map<std::array<int64_t, 2>, std::string> reverse;
    for (auto const& attr : attributes)
        reverse[attr.second] = attr.first;

    for (auto const& obj : objects) {
        std::string descriptive_string;
        auto const& indices = obj.second;
        for (int64_t i = 0; i < static_cast<int64_t>(indices.size()); ++i) {
            auto attr = reverse[{ i, indices[i] }];
            auto it = aliases.find(attr);
            if (it != std::end(aliases))
                attr = it->second;
            if (!attr.empty()) {
                if (i) { descriptive_string += ", "s; }
                descriptive_string += attr;
            }
        }

        desc[obj.first] = descriptive_string;
    }

    return desc;
}

void pencil::categorise(std::string const& label, std::string const& item) {
    if (categories.find(label) == categories.end())
        categories[label] = { static_cast<int>(categories.size()) - 1, 0 };

    if (attributes.find(item) == attributes.end()) {
        attributes[item] = categories[label];
        ++categories[label][1];
    }
}

void pencil::mark(TObject* const object, std::string const& adjective) {
    if (objects.find(object) == objects.end())
        objects[object] = std::vector<int64_t>(categories.size());

    auto attr = attributes[adjective];
    objects[object][attr[0]] = attr[1];
}

void pencil::set_features(int64_t dims) {
    features = std::vector<int64_t>(dims);
    std::iota(std::begin(features), std::end(features), 0);

    if (binary != -1) {
        features[binary] = -1;
        for (int64_t i = binary + 1; i < dims; ++i)
            --features[i];
    }
}

template <typename T>
void pencil::apply(TObject* const obj, int32_t colour, int32_t marker) const {
    if (obj->InheritsFrom(T::Class())) {
        auto cast = static_cast<T*>(obj);
        cast->SetLineColor(colour);
        cast->SetMarkerColor(colour);
        cast->SetMarkerStyle(marker);
    }
}
