<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
  <compound kind="file">
    <name>counts.hpp</name>
    <path>tatami_stats/</path>
    <filename>counts_8hpp.html</filename>
  </compound>
  <compound kind="file">
    <name>grouped_medians.hpp</name>
    <path>tatami_stats/</path>
    <filename>grouped__medians_8hpp.html</filename>
  </compound>
  <compound kind="file">
    <name>grouped_sums.hpp</name>
    <path>tatami_stats/</path>
    <filename>grouped__sums_8hpp.html</filename>
  </compound>
  <compound kind="file">
    <name>medians.hpp</name>
    <path>tatami_stats/</path>
    <filename>medians_8hpp.html</filename>
  </compound>
  <compound kind="file">
    <name>ranges.hpp</name>
    <path>tatami_stats/</path>
    <filename>ranges_8hpp.html</filename>
  </compound>
  <compound kind="file">
    <name>sums.hpp</name>
    <path>tatami_stats/</path>
    <filename>sums_8hpp.html</filename>
    <class kind="struct">tatami_stats::sum::RunningDense</class>
    <class kind="struct">tatami_stats::sum::RunningSparse</class>
    <namespace>tatami_stats::sum</namespace>
  </compound>
  <compound kind="file">
    <name>utils.hpp</name>
    <path>tatami_stats/</path>
    <filename>utils_8hpp.html</filename>
  </compound>
  <compound kind="file">
    <name>variances.hpp</name>
    <path>tatami_stats/</path>
    <filename>variances_8hpp.html</filename>
    <class kind="struct">tatami_stats::variance::RunningDense</class>
    <class kind="struct">tatami_stats::variance::RunningSparse</class>
    <namespace>tatami_stats::variance</namespace>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::sum::RunningDense</name>
    <filename>structtatami__stats_1_1sum_1_1RunningDense.html</filename>
    <templarg>bool skip_nan_</templarg>
    <templarg>typename Output_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningDense</name>
      <anchorfile>structtatami__stats_1_1sum_1_1RunningDense.html</anchorfile>
      <anchor>abec8679bc4e022ea4f660c5888283917</anchor>
      <arglist>(Index_ num, Output_ *sum)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structtatami__stats_1_1sum_1_1RunningDense.html</anchorfile>
      <anchor>a9e972c1b1f73d8896fc5c87907c7ca37</anchor>
      <arglist>(const Value_ *ptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>structtatami__stats_1_1sum_1_1RunningDense.html</anchorfile>
      <anchor>a9b2dd1ad5d36d78bcac94907c8b9acad</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::variance::RunningDense</name>
    <filename>structtatami__stats_1_1variance_1_1RunningDense.html</filename>
    <templarg>bool skip_nan_</templarg>
    <templarg>typename Output_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningDense</name>
      <anchorfile>structtatami__stats_1_1variance_1_1RunningDense.html</anchorfile>
      <anchor>ad0083cbd4b9ca152d36d8b207d3214c5</anchor>
      <arglist>(Index_ num, Output_ *mean, Output_ *variance)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structtatami__stats_1_1variance_1_1RunningDense.html</anchorfile>
      <anchor>a4b5f7f98a85193ccd87ad9b688ad41d2</anchor>
      <arglist>(const Value_ *ptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>structtatami__stats_1_1variance_1_1RunningDense.html</anchorfile>
      <anchor>abe337ec699a0a4009447d058c2cb23ad</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::sum::RunningSparse</name>
    <filename>structtatami__stats_1_1sum_1_1RunningSparse.html</filename>
    <templarg>bool skip_nan_</templarg>
    <templarg>typename Output_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningSparse</name>
      <anchorfile>structtatami__stats_1_1sum_1_1RunningSparse.html</anchorfile>
      <anchor>ac3dca6970c40ec79fabd406b516e4e5b</anchor>
      <arglist>(Index_ num, Output_ *sum, Index_ subtract=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structtatami__stats_1_1sum_1_1RunningSparse.html</anchorfile>
      <anchor>a23ddf9ff9b542217444eee7849c077dc</anchor>
      <arglist>(const Value_ *value, const Index_ *index, Index_ number)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>structtatami__stats_1_1sum_1_1RunningSparse.html</anchorfile>
      <anchor>a6198a3a80019e1be634ebba9f42e30fc</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>tatami_stats::variance::RunningSparse</name>
    <filename>structtatami__stats_1_1variance_1_1RunningSparse.html</filename>
    <templarg>bool skip_nan_</templarg>
    <templarg>typename Output_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningSparse</name>
      <anchorfile>structtatami__stats_1_1variance_1_1RunningSparse.html</anchorfile>
      <anchor>a049c26884ef59175264a806ac5c1ca9d</anchor>
      <arglist>(Index_ num, Output_ *mean, Output_ *variance, Index_ subtract=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>structtatami__stats_1_1variance_1_1RunningSparse.html</anchorfile>
      <anchor>af32ccc83335301ebba7671399e033675</anchor>
      <arglist>(const Value_ *value, const Index_ *index, Index_ number)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>structtatami__stats_1_1variance_1_1RunningSparse.html</anchorfile>
      <anchor>a6672c4e025a63d81aeb8b704e463ac0d</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::sum</name>
    <filename>namespacetatami__stats_1_1sum.html</filename>
    <class kind="struct">tatami_stats::sum::RunningDense</class>
    <class kind="struct">tatami_stats::sum::RunningSparse</class>
    <member kind="function">
      <type>void</type>
      <name>add_neumaier</name>
      <anchorfile>namespacetatami__stats_1_1sum.html</anchorfile>
      <anchor>a064515b44604c4a2db10c73356efa69b</anchor>
      <arglist>(Output_ &amp;sum, Output_ &amp;error, Value_ val)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>compute</name>
      <anchorfile>namespacetatami__stats_1_1sum.html</anchorfile>
      <anchor>a3ff9250c76f369ed345487d4a8291959</anchor>
      <arglist>(const Value_ *ptr, Index_ num)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::variance</name>
    <filename>namespacetatami__stats_1_1variance.html</filename>
    <class kind="struct">tatami_stats::variance::RunningDense</class>
    <class kind="struct">tatami_stats::variance::RunningSparse</class>
    <member kind="function">
      <type>void</type>
      <name>add_welford</name>
      <anchorfile>namespacetatami__stats_1_1variance.html</anchorfile>
      <anchor>aea356cc492fdb031186e8f3e4049a396</anchor>
      <arglist>(Output_ &amp;mean, Output_ &amp;sumsq, Value_ value, Index_ count)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add_welford_zeros</name>
      <anchorfile>namespacetatami__stats_1_1variance.html</anchorfile>
      <anchor>a22ec2e7c62383c5d61ffd4c3f8bc1287</anchor>
      <arglist>(Output_ &amp;mean, Output_ &amp;sumsq, Index_ num_nonzero, Index_ num_all)</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; Output_, Output_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacetatami__stats_1_1variance.html</anchorfile>
      <anchor>a7fc075f377eeccb1ead660f79da7c827</anchor>
      <arglist>(const Value_ *ptr, Index_ num)</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; Output_, Output_ &gt;</type>
      <name>compute</name>
      <anchorfile>namespacetatami__stats_1_1variance.html</anchorfile>
      <anchor>a5827d4ee7566d1a1276f26d4c51b8e16</anchor>
      <arglist>(const Value_ *value, Index_ num_nonzero, Index_ num_all)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Matrix statistics for tatami</title>
    <filename>index.html</filename>
    <docanchor file="index.html" title="Matrix statistics for tatami">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
