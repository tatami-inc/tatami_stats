<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0">
  <compound kind="file">
    <name>count.hpp</name>
    <path>tatami_stats/</path>
    <filename>count_8hpp.html</filename>
    <class kind="struct">tatami_stats::count::Options</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::count</namespace>
  </compound>
  <compound kind="file">
    <name>group_variance.hpp</name>
    <path>tatami_stats/</path>
    <filename>group__variance_8hpp.html</filename>
    <class kind="struct">tatami_stats::group_variance::Options</class>
    <class kind="struct">tatami_stats::group_variance::Buffers</class>
    <class kind="struct">tatami_stats::group_variance::Result</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::group_variance</namespace>
  </compound>
  <compound kind="file">
    <name>grouped_medians.hpp</name>
    <path>tatami_stats/</path>
    <filename>grouped__medians_8hpp.html</filename>
    <class kind="struct">tatami_stats::grouped_medians::Options</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::grouped_medians</namespace>
  </compound>
  <compound kind="file">
    <name>grouped_sums.hpp</name>
    <path>tatami_stats/</path>
    <filename>grouped__sums_8hpp.html</filename>
    <class kind="struct">tatami_stats::grouped_sums::Options</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::grouped_sums</namespace>
  </compound>
  <compound kind="file">
    <name>median.hpp</name>
    <path>tatami_stats/</path>
    <filename>median_8hpp.html</filename>
    <class kind="struct">tatami_stats::median::Options</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::medians</namespace>
  </compound>
  <compound kind="file">
    <name>quantiles.hpp</name>
    <path>tatami_stats/</path>
    <filename>quantiles_8hpp.html</filename>
    <class kind="struct">tatami_stats::quantiles::Options</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::quantiles</namespace>
  </compound>
  <compound kind="file">
    <name>ranges.hpp</name>
    <path>tatami_stats/</path>
    <filename>ranges_8hpp.html</filename>
    <class kind="struct">tatami_stats::ranges::Options</class>
    <class kind="class">tatami_stats::ranges::RunningDense</class>
    <class kind="class">tatami_stats::ranges::RunningSparse</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::ranges</namespace>
  </compound>
  <compound kind="file">
    <name>sums.hpp</name>
    <path>tatami_stats/</path>
    <filename>sums_8hpp.html</filename>
    <class kind="struct">tatami_stats::sums::Options</class>
    <class kind="class">tatami_stats::sums::RunningDense</class>
    <class kind="class">tatami_stats::sums::RunningSparse</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::sums</namespace>
  </compound>
  <compound kind="file">
    <name>tatami_stats.hpp</name>
    <path>tatami_stats/</path>
    <filename>tatami__stats_8hpp.html</filename>
    <namespace>tatami_stats</namespace>
  </compound>
  <compound kind="file">
    <name>utils.hpp</name>
    <path>tatami_stats/</path>
    <filename>utils_8hpp.html</filename>
    <class kind="class">tatami_stats::LocalOutputBuffer</class>
    <class kind="class">tatami_stats::LocalOutputBuffers</class>
    <namespace>tatami_stats</namespace>
  </compound>
  <compound kind="file">
    <name>variance.hpp</name>
    <path>tatami_stats/</path>
    <filename>variance_8hpp.html</filename>
    <class kind="struct">tatami_stats::variance::Options</class>
    <class kind="struct">tatami_stats::variance::Buffers</class>
    <class kind="struct">tatami_stats::variance::Result</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::variances</namespace>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::group_variance::Buffers</name>
    <filename>structtatami__stats_1_1group__variance_1_1Buffers.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>std::vector&lt; Output_ * &gt;</type>
      <name>mean</name>
      <anchorfile>structtatami__stats_1_1group__variance_1_1Buffers.html</anchorfile>
      <anchor>afed5cd7a08497041c4e278b68e1f899d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Output_ * &gt;</type>
      <name>variance</name>
      <anchorfile>structtatami__stats_1_1group__variance_1_1Buffers.html</anchorfile>
      <anchor>a2b2c46ff5a92053ae69652d15529bb36</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::variance::Buffers</name>
    <filename>structtatami__stats_1_1variance_1_1Buffers.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>Output_ *</type>
      <name>mean</name>
      <anchorfile>structtatami__stats_1_1variance_1_1Buffers.html</anchorfile>
      <anchor>a39ca7a33c657d2d82b15b0c21ad36d6b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Output_ *</type>
      <name>variance</name>
      <anchorfile>structtatami__stats_1_1variance_1_1Buffers.html</anchorfile>
      <anchor>a1f1036acbc03fc9fe8c83f99d6b806ac</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>tatami_stats::LocalOutputBuffer</name>
    <filename>classtatami__stats_1_1LocalOutputBuffer.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffer</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffer.html</anchorfile>
      <anchor>ae5cff78f4665bf32d196c74d22d2b0cc</anchor>
      <arglist>(int thread, Index_ start, Index_ length, Output_ *output, Output_ fill)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffer</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffer.html</anchorfile>
      <anchor>a05d3c69fb856cdfb78a65802f4d854d3</anchor>
      <arglist>(int thread, Index_ start, Index_ length, Output_ *output)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffer</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffer.html</anchorfile>
      <anchor>acaa202c720dd81dd06f73e0bc3702b28</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>Output_ *</type>
      <name>data</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffer.html</anchorfile>
      <anchor>a6945cd032378e8cd76a10e6ee0a437da</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const Output_ *</type>
      <name>data</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffer.html</anchorfile>
      <anchor>ae53e9000251661be083d992e9f1cb8be</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transfer</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffer.html</anchorfile>
      <anchor>a64ee29c3a09b95a5d44c5111f357720e</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>tatami_stats::LocalOutputBuffers</name>
    <filename>classtatami__stats_1_1LocalOutputBuffers.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>class GetOutput_</templarg>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffers</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>a22463ea9a1598290159d45fd37675ca9</anchor>
      <arglist>(int thread, Number_ number, Index_ start, Index_ length, GetOutput_ outfun, Output_ fill)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffers</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>aee1914c1fabb9176a81c674f974f5b91</anchor>
      <arglist>(int thread, Number_ number, Index_ start, Index_ length, GetOutput_ outfun)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffers</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>af4c4db4f4578f881b95b7482b4d54ca1</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>std::size_t</type>
      <name>size</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>a4d6a7228d8b4295466eeb7fdb629b59a</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Output_ *</type>
      <name>data</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>a534438174bda372d62582d39c575cd64</anchor>
      <arglist>(std::size_t i)</arglist>
    </member>
    <member kind="function">
      <type>const Output_ *</type>
      <name>data</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>a344bb901aad5d25440d48bb1ad9116a3</anchor>
      <arglist>(std::size_t i) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>transfer</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>a4ca91631f18be6ce71fc58d094dacc8a</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::count::Options</name>
    <filename>structtatami__stats_1_1count_1_1Options.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1count_1_1Options.html</anchorfile>
      <anchor>a0f661e864e19f5a79c86d4698ecdcaad</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::group_variance::Options</name>
    <filename>structtatami__stats_1_1group__variance_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1group__variance_1_1Options.html</anchorfile>
      <anchor>a28839808639fc2b439b0a66e6002e46f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1group__variance_1_1Options.html</anchorfile>
      <anchor>ae9e1d6e9e9c071031ed4176d76396370</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::grouped_medians::Options</name>
    <filename>structtatami__stats_1_1grouped__medians_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1grouped__medians_1_1Options.html</anchorfile>
      <anchor>aaefc94bcd05df52b217ff11c976e124b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1grouped__medians_1_1Options.html</anchorfile>
      <anchor>a445db36471f70bce7b128f585923f0bf</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::grouped_sums::Options</name>
    <filename>structtatami__stats_1_1grouped__sums_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1grouped__sums_1_1Options.html</anchorfile>
      <anchor>a0eefb6cb47a631d15c577f54fbb47941</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1grouped__sums_1_1Options.html</anchorfile>
      <anchor>a842e43a694e59214407423b646bd8e3d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::median::Options</name>
    <filename>structtatami__stats_1_1median_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1median_1_1Options.html</anchorfile>
      <anchor>afa01f69bab33ccc7ce72222e50484a31</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1median_1_1Options.html</anchorfile>
      <anchor>ac97236b3d8db6a8e0ec9c3c3d51a7cab</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::quantiles::Options</name>
    <filename>structtatami__stats_1_1quantiles_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1quantiles_1_1Options.html</anchorfile>
      <anchor>a1132c1939d3a1759873e80152b25c6f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1quantiles_1_1Options.html</anchorfile>
      <anchor>aaa4e6358105ecf531a34b1f22c5014f0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::ranges::Options</name>
    <filename>structtatami__stats_1_1ranges_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1ranges_1_1Options.html</anchorfile>
      <anchor>a15b5deec811e94c441d714abdad16692</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1ranges_1_1Options.html</anchorfile>
      <anchor>a51e1104cc5b2e9be96310f110e1fcd33</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::sums::Options</name>
    <filename>structtatami__stats_1_1sums_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1sums_1_1Options.html</anchorfile>
      <anchor>a9fade20069b7314f5a43315b66a08e7d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1sums_1_1Options.html</anchorfile>
      <anchor>a23cf49b866ad9f5de270dd2ecafad8e5</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::variance::Options</name>
    <filename>structtatami__stats_1_1variance_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1variance_1_1Options.html</anchorfile>
      <anchor>a6b5d05d0c4cf3f4ac49fe9911c1aa205</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1variance_1_1Options.html</anchorfile>
      <anchor>a4baf2dba459c078c57cdbb1311be2fb9</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::group_variance::Result</name>
    <filename>structtatami__stats_1_1group__variance_1_1Result.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>mean</name>
      <anchorfile>structtatami__stats_1_1group__variance_1_1Result.html</anchorfile>
      <anchor>addc30577181d680d1ce1ce22080a4d19</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>variance</name>
      <anchorfile>structtatami__stats_1_1group__variance_1_1Result.html</anchorfile>
      <anchor>ae27777b24cf1a33a4a90ceb22cdbfcf8</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::variance::Result</name>
    <filename>structtatami__stats_1_1variance_1_1Result.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>mean</name>
      <anchorfile>structtatami__stats_1_1variance_1_1Result.html</anchorfile>
      <anchor>a2e50beff5de3f786942606cf6a76eb0b</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>variance</name>
      <anchorfile>structtatami__stats_1_1variance_1_1Result.html</anchorfile>
      <anchor>a81911a61079abe8f9070d177c4f1e0d1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>tatami_stats::ranges::RunningDense</name>
    <filename>classtatami__stats_1_1ranges_1_1RunningDense.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningDense</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningDense.html</anchorfile>
      <anchor>add07104bcd5b8a8c8417ef087643a36e</anchor>
      <arglist>(Index_ num, Output_ *store_min, Output_ *store_max, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningDense.html</anchorfile>
      <anchor>a31eb613e1eb32ab3ec0e6033d1c7cf4e</anchor>
      <arglist>(const Value_ *ptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningDense.html</anchorfile>
      <anchor>a4c62d8472891293e434e329cad403454</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>tatami_stats::sums::RunningDense</name>
    <filename>classtatami__stats_1_1sums_1_1RunningDense.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningDense</name>
      <anchorfile>classtatami__stats_1_1sums_1_1RunningDense.html</anchorfile>
      <anchor>accf7b5a8d3df2682b38ec0cb6851d2c4</anchor>
      <arglist>(Index_ num, Output_ *sum, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classtatami__stats_1_1sums_1_1RunningDense.html</anchorfile>
      <anchor>a1171923aa2b5441d00fb495e8faa6c1e</anchor>
      <arglist>(const Value_ *ptr)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>tatami_stats::ranges::RunningSparse</name>
    <filename>classtatami__stats_1_1ranges_1_1RunningSparse.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningSparse</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningSparse.html</anchorfile>
      <anchor>ac58f2377e5f55cece93f70d36ae0ae9d</anchor>
      <arglist>(Index_ num, Output_ *store_min, Output_ *store_max, bool skip_nan, Index_ subtract=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningSparse.html</anchorfile>
      <anchor>a867b0713e62d612b37fcea2fa8b43f71</anchor>
      <arglist>(const Value_ *value, const Index_ *index, Index_ number)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningSparse.html</anchorfile>
      <anchor>a57b239b04439652337df9db6cecad1e0</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>tatami_stats::sums::RunningSparse</name>
    <filename>classtatami__stats_1_1sums_1_1RunningSparse.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningSparse</name>
      <anchorfile>classtatami__stats_1_1sums_1_1RunningSparse.html</anchorfile>
      <anchor>abcbadacc65ee6a6cee5bc6e668db180f</anchor>
      <arglist>(Output_ *sum, bool skip_nan, Index_ subtract=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classtatami__stats_1_1sums_1_1RunningSparse.html</anchorfile>
      <anchor>a2fede3f8461b31bc9f5da83571bfac39</anchor>
      <arglist>(const Value_ *value, const Index_ *index, Index_ number)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats</name>
    <filename>namespacetatami__stats.html</filename>
    <namespace>tatami_stats::count</namespace>
    <namespace>tatami_stats::group_variance</namespace>
    <namespace>tatami_stats::grouped_medians</namespace>
    <namespace>tatami_stats::grouped_sums</namespace>
    <namespace>tatami_stats::medians</namespace>
    <namespace>tatami_stats::quantiles</namespace>
    <namespace>tatami_stats::ranges</namespace>
    <namespace>tatami_stats::sums</namespace>
    <namespace>tatami_stats::variances</namespace>
    <class kind="class">tatami_stats::LocalOutputBuffer</class>
    <class kind="class">tatami_stats::LocalOutputBuffers</class>
    <member kind="function">
      <type>Number_</type>
      <name>total_groups</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>af1c45653a7d1b140a6fcdd76ceffe32a</anchor>
      <arglist>(const Group_ *group, std::size_t n)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Size_ &gt;</type>
      <name>tabulate_groups</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>ad46c11e3a7611f30d9695e600c5b7dd6</anchor>
      <arglist>(const Group_ *group, Size_ n)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::count</name>
    <filename>namespacetatami__stats_1_1count.html</filename>
    <class kind="struct">tatami_stats::count::Options</class>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1count.html</anchorfile>
      <anchor>a288a7d76957937e08b16b2ef64a441fa</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *const output, Condition_ condition, const Options &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1count.html</anchorfile>
      <anchor>a88c26959501c2b4e5e01aa5115c47c40</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Condition_ condition, const Options &amp;opt)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::group_variance</name>
    <filename>namespacetatami__stats_1_1group__variance.html</filename>
    <class kind="struct">tatami_stats::group_variance::Buffers</class>
    <class kind="struct">tatami_stats::group_variance::Options</class>
    <class kind="struct">tatami_stats::group_variance::Result</class>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1group__variance.html</anchorfile>
      <anchor>a9b81f7b16c2c841a757b852647c75ab2</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, std::size_t num_groups, Buffers&lt; Output_ &gt; &amp;output, const Options &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>Result&lt; Output_ &gt;</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1group__variance.html</anchorfile>
      <anchor>a087510ef814ecaa5247847f635b364cc</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, std::size_t num_groups, const Options &amp;opt)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::grouped_medians</name>
    <filename>namespacetatami__stats_1_1grouped__medians.html</filename>
    <class kind="struct">tatami_stats::grouped_medians::Options</class>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1grouped__medians.html</anchorfile>
      <anchor>a94adfe3299a3636c1227200eefc420c5</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, const GroupSizes_ &amp;group_sizes, Output_ **output, const Options &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1grouped__medians.html</anchorfile>
      <anchor>ace067d4c170730d3939d999bce4f66bd</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, const Options &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1grouped__medians.html</anchorfile>
      <anchor>a229a992062074ed05dba11f4d32d5578</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, const Options &amp;mopt)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::grouped_sums</name>
    <filename>namespacetatami__stats_1_1grouped__sums.html</filename>
    <class kind="struct">tatami_stats::grouped_sums::Options</class>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1grouped__sums.html</anchorfile>
      <anchor>adc7822ac6ec40105185d06a2b83d283e</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, std::size_t num_groups, Output_ **output, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1grouped__sums.html</anchorfile>
      <anchor>a0113733e29712a742ec0e50a6d2e3401</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1grouped__sums.html</anchorfile>
      <anchor>a6a4e926c9181a4ad7eaab0570dafa9af</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, const Options &amp;sopt)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::medians</name>
    <filename>namespacetatami__stats_1_1medians.html</filename>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::quantiles</name>
    <filename>namespacetatami__stats_1_1quantiles.html</filename>
    <class kind="struct">tatami_stats::quantiles::Options</class>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1quantiles.html</anchorfile>
      <anchor>a42d985ea3e2af3e4d4e4dcc37dceb6a2</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const double quantile, Output_ *output, const Options &amp;qopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1quantiles.html</anchorfile>
      <anchor>a491901f6add59fe07bfa18a6731c863f</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const double quantile, const Options &amp;qopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1quantiles.html</anchorfile>
      <anchor>aa9fd95208124d6bd205e7ab0951e954a</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const double quantile, const Options &amp;qopt)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::ranges</name>
    <filename>namespacetatami__stats_1_1ranges.html</filename>
    <class kind="struct">tatami_stats::ranges::Options</class>
    <class kind="class">tatami_stats::ranges::RunningDense</class>
    <class kind="class">tatami_stats::ranges::RunningSparse</class>
    <member kind="function">
      <type>Value_</type>
      <name>direct</name>
      <anchorfile>namespacetatami__stats_1_1ranges.html</anchorfile>
      <anchor>ac651411beec3c6ed51df1c5ff4680551</anchor>
      <arglist>(const Value_ *ptr, Index_ num, bool minimum, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>Value_</type>
      <name>direct</name>
      <anchorfile>namespacetatami__stats_1_1ranges.html</anchorfile>
      <anchor>aebe6fbfef688e635402bcaf1298dacff</anchor>
      <arglist>(const Value_ *value, Index_ num_nonzero, Index_ num_all, bool minimum, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1ranges.html</anchorfile>
      <anchor>a5804f697bfa2e7a3e9b5fdc9c59a561b</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *min_out, Output_ *max_out, const Options &amp;ropt)</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; std::vector&lt; Output_ &gt;, std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1ranges.html</anchorfile>
      <anchor>a97a6ad2ef47fbd5e4c35e0298d953509</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;ropt)</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; std::vector&lt; Output_ &gt;, std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1ranges.html</anchorfile>
      <anchor>a358a512724528f5c2286bc1e13955cbd</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;ropt)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::sums</name>
    <filename>namespacetatami__stats_1_1sums.html</filename>
    <class kind="struct">tatami_stats::sums::Options</class>
    <class kind="class">tatami_stats::sums::RunningDense</class>
    <class kind="class">tatami_stats::sums::RunningSparse</class>
    <member kind="function">
      <type>Output_</type>
      <name>direct</name>
      <anchorfile>namespacetatami__stats_1_1sums.html</anchorfile>
      <anchor>af95ccf6d0eea5492a7ccd6c1c003df92</anchor>
      <arglist>(const Value_ *ptr, Index_ num, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1sums.html</anchorfile>
      <anchor>ae53e59fface3d5ea5cb654598f579af9</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *output, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1sums.html</anchorfile>
      <anchor>aeb23fbdf5e4a5a9b9f2f24f9655cf2ce</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1sums.html</anchorfile>
      <anchor>a789933d544ed5e4a6b4f682f03db56e2</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;sopt)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::variances</name>
    <filename>namespacetatami__stats_1_1variances.html</filename>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Matrix statistics for tatami</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
