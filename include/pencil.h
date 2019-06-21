#ifndef PENCIL_H
#define PENCIL_H

#include <array>
#include <map>
#include <string>
#include <vector>

#include "TObject.h"

class pencil {
  public:
    pencil() : binary(-1) { }

    pencil(pencil const&) = delete;
    pencil& operator=(pencil const&) = delete;
    ~pencil() = default;

    template <typename... T>
    void category(std::string const& label, T const&... items);

    template <typename... T>
    void describe(TObject* const object, T const&... adjectives);

    void set_binary(std::string const& label);

    template <typename T, template <typename...> class U>
    void operator()(T* const obj, U<int64_t> const& attrs) const;

    template <typename T, template <typename...> class U>
    void operator()(T* const obj, U<std::string> const& adjectives) const;

    void sketch();

    void alias(std::string const& label, std::string const& formal);

    std::map<TObject* const, std::string> description() const;

  private:
    void categorise(std::string const& label, std::string const& item);

    void mark(TObject* const object, std::string const& adjective);

    void set_features(int64_t dims);

    template <typename T>
    void apply(TObject* const obj, int32_t colour, int32_t marker) const;

    std::map<TObject* const, std::vector<int64_t>> objects;

    std::map<std::string, std::array<int64_t, 2>> categories;
    std::map<std::string, std::array<int64_t, 2>> attributes;

    std::map<std::string, std::string> aliases;

    std::vector<int64_t> features;
    int64_t binary;
};

#endif /* PENCIL_H */
