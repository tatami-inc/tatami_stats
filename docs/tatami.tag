<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0">
  <compound kind="file">
    <name>count.hpp</name>
    <path>tatami_stats/</path>
    <filename>count_8hpp.html</filename>
    <class kind="struct">tatami_stats::CountOptions</class>
    <namespace>tatami_stats</namespace>
  </compound>
  <compound kind="file">
    <name>group_median.hpp</name>
    <path>tatami_stats/</path>
    <filename>group__median_8hpp.html</filename>
    <class kind="struct">tatami_stats::GroupMedianOptions</class>
    <namespace>tatami_stats</namespace>
  </compound>
  <compound kind="file">
    <name>group_variance.hpp</name>
    <path>tatami_stats/</path>
    <filename>group__variance_8hpp.html</filename>
    <class kind="struct">tatami_stats::GroupVarianceOptions</class>
    <class kind="struct">tatami_stats::GroupVarianceBuffers</class>
    <class kind="struct">tatami_stats::GroupVarianceResult</class>
    <namespace>tatami_stats</namespace>
  </compound>
  <compound kind="file">
    <name>median.hpp</name>
    <path>tatami_stats/</path>
    <filename>median_8hpp.html</filename>
    <class kind="struct">tatami_stats::MedianOptions</class>
    <namespace>tatami_stats</namespace>
  </compound>
  <compound kind="file">
    <name>quantile.hpp</name>
    <path>tatami_stats/</path>
    <filename>quantile_8hpp.html</filename>
    <class kind="struct">tatami_stats::QuantileOptions</class>
    <namespace>tatami_stats</namespace>
  </compound>
  <compound kind="file">
    <name>range.hpp</name>
    <path>tatami_stats/</path>
    <filename>range_8hpp.html</filename>
    <class kind="struct">tatami_stats::RangeOptions</class>
    <class kind="struct">tatami_stats::RangeBuffers</class>
    <class kind="struct">tatami_stats::RangeResult</class>
    <namespace>tatami_stats</namespace>
  </compound>
  <compound kind="file">
    <name>sum.hpp</name>
    <path>tatami_stats/</path>
    <filename>sum_8hpp.html</filename>
    <class kind="struct">tatami_stats::SumOptions</class>
    <namespace>tatami_stats</namespace>
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
    <class kind="struct">tatami_stats::VarianceOptions</class>
    <class kind="struct">tatami_stats::VarianceBuffers</class>
    <class kind="struct">tatami_stats::VarianceResult</class>
    <namespace>tatami_stats</namespace>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::CountOptions</name>
    <filename>structtatami__stats_1_1CountOptions.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1CountOptions.html</anchorfile>
      <anchor>a99d243e055b8bcc2b8151180f3d38da6</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::GroupMedianOptions</name>
    <filename>structtatami__stats_1_1GroupMedianOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1GroupMedianOptions.html</anchorfile>
      <anchor>a95a322285783cf5f23ba5342a223c2e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1GroupMedianOptions.html</anchorfile>
      <anchor>a9cb9eefbb96e15372c0569e0b66424f2</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::GroupSumOptions</name>
    <filename>structtatami__stats_1_1GroupSumOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1GroupSumOptions.html</anchorfile>
      <anchor>a7a150c21b91acc8aabc758dbcca2f2a2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1GroupSumOptions.html</anchorfile>
      <anchor>aa4a3d9b39472a9eef474654e26c2cc36</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::GroupVarianceBuffers</name>
    <filename>structtatami__stats_1_1GroupVarianceBuffers.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>std::vector&lt; Output_ * &gt;</type>
      <name>mean</name>
      <anchorfile>structtatami__stats_1_1GroupVarianceBuffers.html</anchorfile>
      <anchor>a771ae7d9b063b96db8a64f443b591736</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Output_ * &gt;</type>
      <name>variance</name>
      <anchorfile>structtatami__stats_1_1GroupVarianceBuffers.html</anchorfile>
      <anchor>a84e735182fe032cff9c7b8182151f4e1</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::GroupVarianceOptions</name>
    <filename>structtatami__stats_1_1GroupVarianceOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1GroupVarianceOptions.html</anchorfile>
      <anchor>a4c88585c22dad4d741e472cdbec6c233</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1GroupVarianceOptions.html</anchorfile>
      <anchor>af8aae82a226fc7be7903fe040f6d6f0c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::GroupVarianceResult</name>
    <filename>structtatami__stats_1_1GroupVarianceResult.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>mean</name>
      <anchorfile>structtatami__stats_1_1GroupVarianceResult.html</anchorfile>
      <anchor>a281f56a278e69315e854bfdb04165150</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>variance</name>
      <anchorfile>structtatami__stats_1_1GroupVarianceResult.html</anchorfile>
      <anchor>a7b5eed62d9b509969b10857ec86b3a42</anchor>
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
    <name>tatami_stats::MedianOptions</name>
    <filename>structtatami__stats_1_1MedianOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1MedianOptions.html</anchorfile>
      <anchor>a3457394803e4049bc3092284465fcbb8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1MedianOptions.html</anchorfile>
      <anchor>a68e43fb8b128608d02eefc15e4437cda</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::QuantileOptions</name>
    <filename>structtatami__stats_1_1QuantileOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1QuantileOptions.html</anchorfile>
      <anchor>a6d1fe998979cd811775a5f74a3f2cd08</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1QuantileOptions.html</anchorfile>
      <anchor>a44dbdd238da6a3822721e1fb5a17203d</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::RangeBuffers</name>
    <filename>structtatami__stats_1_1RangeBuffers.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>Output_ *</type>
      <name>minimum</name>
      <anchorfile>structtatami__stats_1_1RangeBuffers.html</anchorfile>
      <anchor>afe91ad4486e8aa33d2f4cc78cc3afb9c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Output_ *</type>
      <name>maximum</name>
      <anchorfile>structtatami__stats_1_1RangeBuffers.html</anchorfile>
      <anchor>ab9cd8249399e2e213d62aa3dc9b8f21c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::RangeOptions</name>
    <filename>structtatami__stats_1_1RangeOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1RangeOptions.html</anchorfile>
      <anchor>a11805ebc4488735cf4418c50163dbb85</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1RangeOptions.html</anchorfile>
      <anchor>ae11a188daf7965a81abb5706205f25a3</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::RangeResult</name>
    <filename>structtatami__stats_1_1RangeResult.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>minimum</name>
      <anchorfile>structtatami__stats_1_1RangeResult.html</anchorfile>
      <anchor>afc9b54e80185c8740486f6d287834bae</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>maximum</name>
      <anchorfile>structtatami__stats_1_1RangeResult.html</anchorfile>
      <anchor>abd514236d2132c48d2c4aed304969467</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::SumOptions</name>
    <filename>structtatami__stats_1_1SumOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1SumOptions.html</anchorfile>
      <anchor>af03c11adfc70b9efed3d42ab205dc6c2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1SumOptions.html</anchorfile>
      <anchor>a2292bbf24fd78e48fbe7e5416039b9fa</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::VarianceBuffers</name>
    <filename>structtatami__stats_1_1VarianceBuffers.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>Output_ *</type>
      <name>mean</name>
      <anchorfile>structtatami__stats_1_1VarianceBuffers.html</anchorfile>
      <anchor>a11227ea52547a2ea7c0b88b3a90ba241</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>Output_ *</type>
      <name>variance</name>
      <anchorfile>structtatami__stats_1_1VarianceBuffers.html</anchorfile>
      <anchor>ae82682f6f261a5f8361a7e41b435d06f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::VarianceOptions</name>
    <filename>structtatami__stats_1_1VarianceOptions.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1VarianceOptions.html</anchorfile>
      <anchor>a7720cc1ed95e03d7945183d6978926ea</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1VarianceOptions.html</anchorfile>
      <anchor>abde48b60fc88c3373638dddc323d7f97</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::VarianceResult</name>
    <filename>structtatami__stats_1_1VarianceResult.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="variable">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>mean</name>
      <anchorfile>structtatami__stats_1_1VarianceResult.html</anchorfile>
      <anchor>af7eefe392238647ffaf7454b17014c9a</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>variance</name>
      <anchorfile>structtatami__stats_1_1VarianceResult.html</anchorfile>
      <anchor>aa48ad71e29a9ff3e7928a89929a42e95</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats</name>
    <filename>namespacetatami__stats.html</filename>
    <class kind="struct">tatami_stats::CountOptions</class>
    <class kind="struct">tatami_stats::GroupMedianOptions</class>
    <class kind="struct">tatami_stats::GroupSumOptions</class>
    <class kind="struct">tatami_stats::GroupVarianceBuffers</class>
    <class kind="struct">tatami_stats::GroupVarianceOptions</class>
    <class kind="struct">tatami_stats::GroupVarianceResult</class>
    <class kind="class">tatami_stats::LocalOutputBuffer</class>
    <class kind="class">tatami_stats::LocalOutputBuffers</class>
    <class kind="struct">tatami_stats::MedianOptions</class>
    <class kind="struct">tatami_stats::QuantileOptions</class>
    <class kind="struct">tatami_stats::RangeBuffers</class>
    <class kind="struct">tatami_stats::RangeOptions</class>
    <class kind="struct">tatami_stats::RangeResult</class>
    <class kind="struct">tatami_stats::SumOptions</class>
    <class kind="struct">tatami_stats::VarianceBuffers</class>
    <class kind="struct">tatami_stats::VarianceOptions</class>
    <class kind="struct">tatami_stats::VarianceResult</class>
    <member kind="function">
      <type>void</type>
      <name>count</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a9e8763c74e8b0cfba7f1dedb1c0fbb24</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *const output, Condition_ condition, const CountOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>count</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a6fe75356dd7e0b752d0b67d25e176088</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Condition_ condition, const CountOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>group_median</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>ab70fac32c7dab5f22803aced34612227</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *const group, const std::size_t num_groups, std::vector&lt; Output_ * &gt; &amp;output, const GroupMedianOptions &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>group_median</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>ab0540c22b358d31f0f16dab8a113599a</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *const group, const std::size_t num_groups, const GroupMedianOptions &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>group_sum</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a6cc6c591b9a26a77ca7db585760ab6c0</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, const std::size_t num_groups, std::vector&lt; Output_ * &gt; &amp;output, const GroupSumOptions &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>group_sum</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a2a8398462d0d1bffe73df4ba398e5bcf</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, const std::size_t num_groups, const GroupSumOptions &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>group_variance</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>ac7476ce5e6db67ed8d137d14077d6a10</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *const group, const std::size_t num_groups, GroupVarianceBuffers&lt; Output_ &gt; &amp;output, const GroupVarianceOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>GroupVarianceResult&lt; Output_ &gt;</type>
      <name>group_variance</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>ab537972be4ae242bbeb1ee6707e5a794</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *const group, const std::size_t num_groups, const GroupVarianceOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>median</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>aebf2d6f697c1572992203b6a891e1e20</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *const output, const MedianOptions &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>median</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>ab2fe9e8e668c05880dbd6fca9215c079</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const MedianOptions &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>quantile</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a1133dbab46ab7349216912c98029821b</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const double prob, Output_ *const output, const QuantileOptions &amp;qopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>quantile</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a32616b33c9eec7599005585699335e0f</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const double prob, const QuantileOptions &amp;qopt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>range</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a97bd6c0090005d7070c59c3e8585868d</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, RangeBuffers&lt; Output_ &gt; &amp;output, const RangeOptions &amp;ropt)</arglist>
    </member>
    <member kind="function">
      <type>RangeResult&lt; Output_ &gt;</type>
      <name>range</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>abd63e1c877897a7ed9558403fc116aae</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const RangeOptions &amp;ropt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sum</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>adfaa890c5bf7717c30ee0ae65c840f12</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *output, const SumOptions &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>sum</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>af3ad9a0e58c20c7b5012d6b202e0bba0</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const SumOptions &amp;sopt)</arglist>
    </member>
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
    <member kind="function">
      <type>void</type>
      <name>variance</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a8c19d4ae7dfe65e5701f46e1e003ec34</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, VarianceBuffers&lt; Output_ &gt; &amp;output, const VarianceOptions &amp;vopt)</arglist>
    </member>
    <member kind="function">
      <type>VarianceResult&lt; Output_ &gt;</type>
      <name>variance</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>af1f7492ecdf7283e2e8bf69d4944d5ad</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const VarianceOptions &amp;vopt)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Matrix statistics for tatami</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
