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
    <name>group_sum.hpp</name>
    <path>tatami_stats/</path>
    <filename>group__sum_8hpp.html</filename>
    <class kind="struct">tatami_stats::GroupSumOptions</class>
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
      <anchor>a7f9ead1e0237477c8ff009eea44dcc5b</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *const group, const std::size_t num_groups, std::vector&lt; Output_ * &gt; &amp;output, const GroupMedianOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>group_median</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a34a64e23fbbb053cf65ff2c056ff2623</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *const group, const std::size_t num_groups, const GroupMedianOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>group_sum</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a914d2af37165ace4dcbe0a54ee065fd7</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, const std::size_t num_groups, std::vector&lt; Output_ * &gt; &amp;output, const GroupSumOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>group_sum</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>aed9cbd9b45cde860ce180bd21760d3e9</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, const std::size_t num_groups, const GroupSumOptions &amp;opt)</arglist>
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
      <anchor>a2d3b162e10b92701de86a48d22c1b0de</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *const output, const MedianOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>median</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>abf74c1fe4db53be6ea36b81525249493</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const MedianOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>quantile</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a57171c02a9c1f651e396ee60df2a44d3</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const double prob, Output_ *const output, const QuantileOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>quantile</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a2234f36e19560dcc655246c6cb9d0410</anchor>
      <arglist>(const bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const double prob, const QuantileOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>range</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a7744693451913c2be630f12ecfb9a201</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, RangeBuffers&lt; Output_ &gt; &amp;output, const RangeOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>RangeResult&lt; Output_ &gt;</type>
      <name>range</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a6f936a60c798ab2c134383a421e079c3</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const RangeOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sum</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>ac9c9b11bc8af6e6a1497cc088b17f054</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *output, const SumOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>sum</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>aeadf1ce1d2d7b95d5085a6ac7a49864b</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const SumOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>variance</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>af5b0ad69b65dafbeb145903572b421df</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, VarianceBuffers&lt; Output_ &gt; &amp;output, const VarianceOptions &amp;opt)</arglist>
    </member>
    <member kind="function">
      <type>VarianceResult&lt; Output_ &gt;</type>
      <name>variance</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>af737713efbcb6ce5ed8b1dca03b71a4b</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const VarianceOptions &amp;opt)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Matrix statistics for tatami</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
