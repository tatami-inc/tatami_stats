<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.12.0">
  <compound kind="file">
    <name>counts.hpp</name>
    <path>tatami_stats/</path>
    <filename>counts_8hpp.html</filename>
    <class kind="struct">tatami_stats::counts::nan::Options</class>
    <class kind="struct">tatami_stats::counts::zero::Options</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::counts</namespace>
    <namespace>tatami_stats::counts::nan</namespace>
    <namespace>tatami_stats::counts::zero</namespace>
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
    <name>grouped_variances.hpp</name>
    <path>tatami_stats/</path>
    <filename>grouped__variances_8hpp.html</filename>
    <class kind="struct">tatami_stats::grouped_variances::Options</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::grouped_variances</namespace>
  </compound>
  <compound kind="file">
    <name>medians.hpp</name>
    <path>tatami_stats/</path>
    <filename>medians_8hpp.html</filename>
    <class kind="struct">tatami_stats::medians::Options</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::medians</namespace>
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
    <name>variances.hpp</name>
    <path>tatami_stats/</path>
    <filename>variances_8hpp.html</filename>
    <class kind="struct">tatami_stats::variances::Options</class>
    <class kind="class">tatami_stats::variances::RunningDense</class>
    <class kind="class">tatami_stats::variances::RunningSparse</class>
    <namespace>tatami_stats</namespace>
    <namespace>tatami_stats::variances</namespace>
  </compound>
  <compound kind="class">
    <name>tatami_stats::LocalOutputBuffer</name>
    <filename>classtatami__stats_1_1LocalOutputBuffer.html</filename>
    <templarg>typename Output_</templarg>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffer</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffer.html</anchorfile>
      <anchor>adf9491eb22741367c4aa2ccc1cd13487</anchor>
      <arglist>(std::size_t thread, Index_ start, Index_ length, Output_ *output, Output_ fill)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffer</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffer.html</anchorfile>
      <anchor>af34eec4db79da3550fb3326024ec6246</anchor>
      <arglist>(std::size_t thread, Index_ start, Index_ length, Output_ *output)</arglist>
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
      <anchor>a10ceb3fe5c08bd6bf82a3fb28a51884d</anchor>
      <arglist>(std::size_t thread, std::size_t number, Index_ start, Index_ length, GetOutput_ outfun, Output_ fill)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffers</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>ad08acff7257b3df3febbe042672432f9</anchor>
      <arglist>(std::size_t thread, std::size_t number, Index_ start, Index_ length, GetOutput_ outfun)</arglist>
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
    <name>tatami_stats::counts::nan::Options</name>
    <filename>structtatami__stats_1_1counts_1_1nan_1_1Options.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1counts_1_1nan_1_1Options.html</anchorfile>
      <anchor>a792f980aeabb8086664b72444156aa8e</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::counts::zero::Options</name>
    <filename>structtatami__stats_1_1counts_1_1zero_1_1Options.html</filename>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1counts_1_1zero_1_1Options.html</anchorfile>
      <anchor>afb95b1354e56fcf8f19f9e96dd88ee39</anchor>
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
    <name>tatami_stats::grouped_variances::Options</name>
    <filename>structtatami__stats_1_1grouped__variances_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1grouped__variances_1_1Options.html</anchorfile>
      <anchor>a00d2ca136b913d98aef0e918c49f07c8</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1grouped__variances_1_1Options.html</anchorfile>
      <anchor>aa8b1c39c58ba7bf063a2dd281856be42</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::medians::Options</name>
    <filename>structtatami__stats_1_1medians_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1medians_1_1Options.html</anchorfile>
      <anchor>a2fa7e058d6053d7f593554ce67a34145</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1medians_1_1Options.html</anchorfile>
      <anchor>a19cd552a4e637e6f82bf145e8e94b9a6</anchor>
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
    <name>tatami_stats::variances::Options</name>
    <filename>structtatami__stats_1_1variances_1_1Options.html</filename>
    <member kind="variable">
      <type>bool</type>
      <name>skip_nan</name>
      <anchorfile>structtatami__stats_1_1variances_1_1Options.html</anchorfile>
      <anchor>aeb3ee96b5c18332bd57e0633c3593c53</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>int</type>
      <name>num_threads</name>
      <anchorfile>structtatami__stats_1_1variances_1_1Options.html</anchorfile>
      <anchor>a067e87951e90ca4510b110c13efe56cb</anchor>
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
    <name>tatami_stats::variances::RunningDense</name>
    <filename>classtatami__stats_1_1variances_1_1RunningDense.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningDense</name>
      <anchorfile>classtatami__stats_1_1variances_1_1RunningDense.html</anchorfile>
      <anchor>ae4305fb083a270fa5f5800e8a5bdb140</anchor>
      <arglist>(Index_ num, Output_ *mean, Output_ *variance, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classtatami__stats_1_1variances_1_1RunningDense.html</anchorfile>
      <anchor>a13d93ffc98acfd3f7eb53265ef694c07</anchor>
      <arglist>(const Value_ *ptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>classtatami__stats_1_1variances_1_1RunningDense.html</anchorfile>
      <anchor>a389232abda6b2ed3218e5cb648d1f33c</anchor>
      <arglist>()</arglist>
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
  <compound kind="class">
    <name>tatami_stats::variances::RunningSparse</name>
    <filename>classtatami__stats_1_1variances_1_1RunningSparse.html</filename>
    <templarg>typename Output_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningSparse</name>
      <anchorfile>classtatami__stats_1_1variances_1_1RunningSparse.html</anchorfile>
      <anchor>acb56694c71bb6155a3ad90e5e1437347</anchor>
      <arglist>(Index_ num, Output_ *mean, Output_ *variance, bool skip_nan, Index_ subtract=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classtatami__stats_1_1variances_1_1RunningSparse.html</anchorfile>
      <anchor>ad998372f22ce80165cc39a8bfcb74108</anchor>
      <arglist>(const Value_ *value, const Index_ *index, Index_ number)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>classtatami__stats_1_1variances_1_1RunningSparse.html</anchorfile>
      <anchor>a9ab4ac3ef21c82557e7292dc38d9e504</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats</name>
    <filename>namespacetatami__stats.html</filename>
    <namespace>tatami_stats::counts</namespace>
    <namespace>tatami_stats::grouped_medians</namespace>
    <namespace>tatami_stats::grouped_sums</namespace>
    <namespace>tatami_stats::grouped_variances</namespace>
    <namespace>tatami_stats::medians</namespace>
    <namespace>tatami_stats::ranges</namespace>
    <namespace>tatami_stats::sums</namespace>
    <namespace>tatami_stats::variances</namespace>
    <class kind="class">tatami_stats::LocalOutputBuffer</class>
    <class kind="class">tatami_stats::LocalOutputBuffers</class>
    <member kind="function">
      <type>std::size_t</type>
      <name>total_groups</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>a020f3eb0fea95ecf56df74c6a6cd672d</anchor>
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
    <name>tatami_stats::counts</name>
    <filename>namespacetatami__stats_1_1counts.html</filename>
    <namespace>tatami_stats::counts::nan</namespace>
    <namespace>tatami_stats::counts::zero</namespace>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1counts.html</anchorfile>
      <anchor>a6fab7e429397cda0c52501f3d1280d40</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *output, int num_threads, Condition_ condition)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::counts::nan</name>
    <filename>namespacetatami__stats_1_1counts_1_1nan.html</filename>
    <class kind="struct">tatami_stats::counts::nan::Options</class>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1nan.html</anchorfile>
      <anchor>ade8f471e6ffb65979c47f9306a462c91</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *output, const Options &amp;nopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1nan.html</anchorfile>
      <anchor>a0e87c2576b55646820eb10791ab50eae</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;nopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1nan.html</anchorfile>
      <anchor>aab621a0818e57bbc76831d38bcace181</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1nan.html</anchorfile>
      <anchor>a2ef755d1592efd72f35c644c35064a75</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;nopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1nan.html</anchorfile>
      <anchor>a0e659e23cfea20741ce3b6c0da9a9f58</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::counts::zero</name>
    <filename>namespacetatami__stats_1_1counts_1_1zero.html</filename>
    <class kind="struct">tatami_stats::counts::zero::Options</class>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1zero.html</anchorfile>
      <anchor>a817f100b20ceb21ebeac28e6880160f5</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *output, const Options &amp;zopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1zero.html</anchorfile>
      <anchor>af559d2616322e068b400d36cd4a28bde</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;zopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1zero.html</anchorfile>
      <anchor>a9223d94e5cc7fecbcc39ffa736aa42aa</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1zero.html</anchorfile>
      <anchor>aa13dd1495b40349531c65f030b6bd059</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;zopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1zero.html</anchorfile>
      <anchor>a2d689bba39823ea82569ebf05445cbb8</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat)</arglist>
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
    <name>tatami_stats::grouped_variances</name>
    <filename>namespacetatami__stats_1_1grouped__variances.html</filename>
    <class kind="struct">tatami_stats::grouped_variances::Options</class>
    <member kind="function">
      <type>void</type>
      <name>direct</name>
      <anchorfile>namespacetatami__stats_1_1grouped__variances.html</anchorfile>
      <anchor>a0ca9d67874e780b6c83dfc77a252f567</anchor>
      <arglist>(const Value_ *ptr, Index_ num, const Group_ *group, std::size_t num_groups, const Index_ *group_size, Output_ *output_means, Output_ *output_variances, bool skip_nan, Index_ *valid_group_size)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>direct</name>
      <anchorfile>namespacetatami__stats_1_1grouped__variances.html</anchorfile>
      <anchor>a5949219e7545b96e549fe12bf02e29c4</anchor>
      <arglist>(const Value_ *value, const Index_ *index, Index_ num_nonzero, const Group_ *group, std::size_t num_groups, const Index_ *group_size, Output_ *output_means, Output_ *output_variances, Index_ *output_nonzero, bool skip_nan, Index_ *valid_group_size)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1grouped__variances.html</anchorfile>
      <anchor>a3eac3787693b4533e2afa7a7c01852eb</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, std::size_t num_groups, const Index_ *group_size, Output_ **output, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1grouped__variances.html</anchorfile>
      <anchor>abd3490a0beece305c896f35cc86f53e6</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1grouped__variances.html</anchorfile>
      <anchor>a47d05526a9980d64b58152a2d7db6fa2</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Group_ *group, const Options &amp;sopt)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::medians</name>
    <filename>namespacetatami__stats_1_1medians.html</filename>
    <class kind="struct">tatami_stats::medians::Options</class>
    <member kind="function">
      <type>Output_</type>
      <name>direct</name>
      <anchorfile>namespacetatami__stats_1_1medians.html</anchorfile>
      <anchor>ad805e96ccca9615107f7360788f24a44</anchor>
      <arglist>(Value_ *ptr, Index_ num, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>direct</name>
      <anchorfile>namespacetatami__stats_1_1medians.html</anchorfile>
      <anchor>a646feafa77f4f32dbcb2d58f76e6bd6a</anchor>
      <arglist>(Value_ *value, Index_ num_nonzero, Index_ num_all, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1medians.html</anchorfile>
      <anchor>ae1c82c1c78589f70f873f6e66827e9a4</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *output, const medians::Options &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1medians.html</anchorfile>
      <anchor>ae23c5bcce3513e9b362a84497d138f40</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1medians.html</anchorfile>
      <anchor>ab70115fd094f9395a13bd3d18418df51</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;mopt)</arglist>
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
    <class kind="struct">tatami_stats::variances::Options</class>
    <class kind="class">tatami_stats::variances::RunningDense</class>
    <class kind="class">tatami_stats::variances::RunningSparse</class>
    <member kind="function">
      <type>std::pair&lt; Output_, Output_ &gt;</type>
      <name>direct</name>
      <anchorfile>namespacetatami__stats_1_1variances.html</anchorfile>
      <anchor>a321f88af41ac5ef639c72c57a067bd71</anchor>
      <arglist>(const Value_ *value, Index_ num_nonzero, Index_ num_all, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; Output_, Output_ &gt;</type>
      <name>direct</name>
      <anchorfile>namespacetatami__stats_1_1variances.html</anchorfile>
      <anchor>a001e16afe34b07f6981b674c6a840fa9</anchor>
      <arglist>(const Value_ *ptr, Index_ num, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1variances.html</anchorfile>
      <anchor>a52fd7f235d9c9d0721210f3c8b5c503a</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, Output_ *output, const Options &amp;vopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1variances.html</anchorfile>
      <anchor>a1b78af51c1130641ff940ce5bbc8b645</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;vopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1variances.html</anchorfile>
      <anchor>a6788d39d4ab53679786e67794d7a6578</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; &amp;mat, const Options &amp;vopt)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Matrix statistics for tatami</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
