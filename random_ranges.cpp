#include <boost/program_options.hpp>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <map>
#include <unordered_set>
#include <chrono>
#include "sparsepp/spp.h"

class Actor
{
public:
  virtual void prepare(const std::vector<uint32_t>&) =0;
  virtual size_t run(const std::vector<uint32_t>&) =0;
  virtual std::string signature() =0;
};

// just print it out
class Visual: public Actor
{
  void
  prepare(const std::vector<uint32_t>& v) override
  {
    for (auto el : v)
    {
      std::cout << el << std::endl;
    }
  }

  size_t
  run(const std::vector<uint32_t>&) override
  {
    return 0;
  }

  std::string
  signature() override
  {
    return "VISUAL";
  }
};

class Stdmap: public Actor
{
  void
  prepare(const std::vector<uint32_t>& v) override
  {
    int32_t prev_el = v[0];
    int32_t start_range = v[0];

    std::for_each (
      v.begin()+1, v.end(), [&](int32_t el)

      {
        if (el == prev_el + 1)
        {
          prev_el++;
        }
        else
        {
          mp.insert({start_range, prev_el});
          start_range = prev_el = el;
        }
      }
                   );

    if (start_range != prev_el)
    {
      mp.insert({start_range, prev_el});
    }
    std::cout << mp.size() << " ranges" << std::endl;
  }

  size_t
  run(const std::vector<uint32_t>& requested) override
  {
    size_t total_found = 0;

    for (uint32_t v : requested)
    {
      bool found = false;
      auto it = mp.upper_bound(v);
      // we are looking for left side

      if (it != mp.begin())
      {
        assert(!mp.empty());
        found = v <= (*--it).second;
      }
#ifndef NDEBUG
      std::cout << (found ? "found " : "not found ") << v << std::endl;
#endif
      if (found)
      {
        total_found++;
      }
    }
    return total_found;
  }

  std::string
  signature() override
  {
    return "STDMAP";
  }

  //       from      to
  std::map<uint32_t, uint32_t> mp;
};

class Binsearch: public Actor
{
  void
  prepare(const std::vector<uint32_t>& v) override
  {
    int32_t prev_el = v[0];
    int32_t start_range = v[0];

    std::for_each (
      v.begin()+1, v.end(), [&](int32_t el)

      {
        if (el == prev_el + 1)
        {
          prev_el++;
        }
        else
        {
          mp.push_back({start_range, prev_el});
          start_range = prev_el = el;
        }
      }
                   );

    if (start_range != prev_el)
    {
      mp.push_back({start_range, prev_el});
    }
  }

  size_t
  run(const std::vector<uint32_t>& requested) override
  {
    size_t total_found = 0;
    for (uint32_t v : requested)
    {
      auto it = std::upper_bound(begin(mp), end(mp), Range{v, 0});
      bool found = it != end(mp) && v == (*it).first;
      // we are looking for left side

      if (it != begin(mp))
      {
        assert(!mp.empty());
        found = found || v <= (*--it).second;
      }
#ifndef NDEBUG
      std::cout << (found ? "found " : "not found ") << v << std::endl;
#endif
      if (found)
      {
        total_found++;
      }
    }
    return total_found;
  }

  std::string
  signature() override
  {
    return "BINSEARCH";
  }

protected:
  using Range = std::pair<uint32_t, uint32_t>;
  std::vector<Range> mp;
};

class Binsearchfast: public Binsearch
{
  size_t
  run(const std::vector<uint32_t>& requested) override
  {
    size_t total_found = 0;
    for (uint32_t v : requested)
    {
      size_t low = fast_upper_bound3(mp, Range{v, 0});
      // std::cout << "low=" << low << std::endl;

      // we are looking for left side
      bool found = v == mp[low].first;
      if (low)
      {
        found = found || v <= mp[low-1].second;
      }

#ifndef NDEBUG
      std::cout << (found ? "found " : "not found ") << v << std::endl;
#endif
      if (found)
      {
        total_found++;
      }
    }
    return total_found;
  }

  size_t fast_upper_bound3(const std::vector<Range>& vec, Range value)
  {
    size_t size = vec.size();
    size_t low = 0;

    while (size > 0)
    {
      size_t half = size / 2;
      size_t other_half = size - half;
      size_t probe = low + half;
      size_t other_low = low + other_half;
      Range v = vec[probe];
      size = half;


      // It is Ok to use std::pair compare - no performance difference
      low = v.first >= value.first ? low : other_low;
    }

    return low;
  }



  std::string
  signature() override
  {
    return "BINSEARCHFAST";
  }
};

class Unorderedset: public Actor
{
  void
  prepare(const std::vector<uint32_t>& v) override
  {
    mp.insert(begin(v), end(v));
  }

  size_t
  run(const std::vector<uint32_t>& requested) override
  {
    size_t total_found = 0;
    for (uint32_t v : requested)
    {
      bool found = mp.find(v) != end(mp);
#ifndef NDEBUG
      std::cout << (found ? "found " : "not found ") << v << std::endl;
#endif
      if (found)
      {
        total_found++;
      }
    }
    return total_found;
  }

  std::string
  signature() override
  {
    return "UNORDEREDSET";
  }

  std::unordered_set<uint32_t> mp;
};

class Sparsehashset: public Actor
{
  void
  prepare(const std::vector<uint32_t>& v) override
  {
    mp.insert(begin(v), end(v));
  }

  size_t
  run(const std::vector<uint32_t>& requested) override
  {
    size_t total_found = 0;
    for (uint32_t v : requested)
    {
      bool found = mp.find(v) != end(mp);
#ifndef NDEBUG
      std::cout << (found ? "found " : "not found ") << v << std::endl;
#endif
      if (found)
      {
        total_found++;
      }
    }
    return total_found;
  }

  std::string
  signature() override
  {
    return "SPARSEHASHSET";
  }

  spp::sparse_hash_set<uint32_t> mp;
};


namespace po = boost::program_options;
po::variables_map vm;

int
main(int argc, char** argv)
{
  po::options_description desc("Allowed options");
  desc.add_options()("help", "produce help message")
    ("debug", "debug output")
    ("known-ips-file",
      po::value<std::string>()->default_value("ips.data"),
      "File with known IP per line")
    ("requested-ips-file",
      po::value<std::string>()->default_value("requested_ips.data"),
      "File with IPs of the question, both known and not known")
    ("iterations",
      po::value<size_t>()->default_value(1000),
      "Number of times we go through requested-ips-file");

  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);

  if (vm.count("help"))
  {
    std::cout << desc << "\n";
    return 1;
  }

  std::vector<uint32_t> known_ips;
  std::ifstream known_ips_file(vm["known-ips-file"].as<std::string>());
  std::for_each( std::istream_iterator<std::string>(known_ips_file),
    std::istream_iterator<std::string>(),
    [&known_ips](const std::string& s)
    {
      known_ips.push_back(std::stoul(s));
    }

    );


  std::vector<std::shared_ptr<Actor>> actors =
    {
      // new Visual(),
      std::make_shared<Stdmap>(),
      std::make_shared<Binsearch>(),
      std::make_shared<Binsearchfast>(),
      std::make_shared<Unorderedset>(),
      std::make_shared<Sparsehashset>()
    };

  std::cout << "## preparing actors" << std::endl;
  for (auto& actor : actors)
  {
    std::cout << actor->signature() << std::endl;
    actor->prepare(known_ips);
  }

  std::vector<uint32_t> requested_ips;
  std::ifstream requested_ips_file(vm["requested-ips-file"].as<std::string>());
  std::for_each( std::istream_iterator<std::string>(requested_ips_file),
    std::istream_iterator<std::string>(),
    [&requested_ips](const std::string& s)
    {
      requested_ips.push_back(std::stoul(s));

    }

    );

  std::cout << "## running actors" << std::endl;
  for (auto& actor : actors)
  {
    std::cout << actor->signature() << std::endl;
    auto stamp = std::chrono::steady_clock::now();

    size_t total_found = 0;
    for (size_t i = 0; i < vm["iterations"].as<size_t>(); ++i)
    {
      total_found += actor->run(requested_ips);
    }

    time_t elapsed = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - stamp).count();

    std::cout << "took " << elapsed << " microseconds, found " << total_found << std::endl;
  }
}
