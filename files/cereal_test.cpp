// cereal_test.cpp - sanity test for an installed cereal
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/vector.hpp>

#include <cstdio>
#include <map>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

static int failures = 0;

static void check(bool ok, const char* what) {
  std::printf("%s: %s\n", ok ? "ok  " : "FAIL", what);
  if (!ok) ++failures;
}

struct Record {
  int id = 0;
  double score = 0.0;
  std::string name;
  std::vector<int> values;
  std::map<std::string, int> counts;
  std::tuple<int, double, std::string> triple;

  template <class Archive>
  void serialize(Archive& ar) {
    ar(CEREAL_NVP(id), CEREAL_NVP(score), CEREAL_NVP(name),
       CEREAL_NVP(values), CEREAL_NVP(counts), CEREAL_NVP(triple));
  }

  bool operator==(const Record& o) const {
    return id == o.id && score == o.score && name == o.name &&
           values == o.values && counts == o.counts && triple == o.triple;
  }
};

static Record make_record() {
  Record r;
  r.id = 42;
  r.score = 3.14159;
  r.name = "cereal";
  r.values = {1, 2, 3, 5, 8, 13};
  r.counts = {{"a", 1}, {"b", 2}, {"c", 3}};
  r.triple = std::make_tuple(7, 2.5, std::string("tuple"));
  return r;
}

int main() {
  const Record original = make_record();

  // Binary round trip. The Record contains a std::tuple, so this goes
  // through cereal/types/tuple.hpp, the header the clang patch fixes.
  {
    std::stringstream ss;
    {
      cereal::BinaryOutputArchive out(ss);
      out(original);
    }
    Record loaded;
    {
      cereal::BinaryInputArchive in(ss);
      in(loaded);
    }
    check(loaded == original, "binary round trip (with std::tuple)");
  }

  // JSON round trip. The output archive writes on destruction, so scope it.
  {
    std::stringstream ss;
    {
      cereal::JSONOutputArchive out(ss);
      out(cereal::make_nvp("record", original));
    }
    const std::string text = ss.str();
    check(text.find("\"name\": \"cereal\"") != std::string::npos,
          "JSON output contains expected field");
    Record loaded;
    {
      cereal::JSONInputArchive in(ss);
      in(cereal::make_nvp("record", loaded));
    }
    check(loaded == original, "JSON round trip");
  }

  // A tuple on its own, including an empty-ish and a longer one.
  {
    std::stringstream ss;
    const auto t = std::make_tuple(1, 2.0, std::string("x"), 4, 5.5f);
    {
      cereal::BinaryOutputArchive out(ss);
      out(t);
    }
    std::tuple<int, double, std::string, int, float> back;
    {
      cereal::BinaryInputArchive in(ss);
      in(back);
    }
    check(back == t, "five-element tuple");
  }

  std::printf(failures == 0 ? "PASS\n" : "FAILED (%d)\n", failures);
  return failures == 0 ? 0 : 1;
}
