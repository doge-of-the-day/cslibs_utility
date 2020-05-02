#include <gtest/gtest.h>

#include <cslibs_utility/logger/csv_reader.hpp>
#include <cslibs_utility/logger/csv_writer.hpp>

TEST(Test_cslibs_utility, csvWriterReader) {
  using writer_t = cslibs_utility::logger::CSVWriter<int, double>;
  using reader_t = cslibs_utility::logger::CSVReader<int, double>;

  std::vector<double> d{0.0, 1.0, 2.0, 3.0, 4.0};
  std::vector<int> i{0, 1, 2, 3, 4};
  writer_t::header_t h{"int", "double"};
  {
    writer_t w{h, "/tmp/cslibs_utility_test.csv"};
    for (std::size_t j = 0; j < 5; ++j) {
      w.write(i[j], d[j]);
    }
  }

  reader_t r{"/tmp/cslibs_utility_test.csv", true};
  EXPECT_TRUE(r.hasHeader());
  EXPECT_EQ(r.getHeader().front(), h.front());
  EXPECT_EQ(r.getHeader().back(), h.back());

  for(std::size_t s = 0 ; s < 5 ; ++s) {
      const auto &entry = r.getData()[s];
      EXPECT_EQ(std::get<0>(entry), i[s]);
      EXPECT_EQ(std::get<1>(entry), d[s]);
  }
}

int main(int argc, char *argv[]) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
