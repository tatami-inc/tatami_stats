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
    <namespace>tatami_stats::median</namespace>
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
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::median</name>
    <filename>namespacetatami__stats_1_1median.html</filename>
    <member kind="function">
      <type>Output_</type>
      <name>compute</name>
      <anchorfile>namespacetatami__stats_1_1median.html</anchorfile>
      <anchor>a9747d128e5ffc02215e8d9b13dcff91e</anchor>
      <arglist>(Value_ *ptr, Index_ num)</arglist>
    </member>
    <member kind="function">
      <type>Output_</type>
      <name>compute</name>
      <anchorfile>namespacetatami__stats_1_1median.html</anchorfile>
      <anchor>a7c45e9e0ec266e59d9f5fba9d02244a1</anchor>
      <arglist>(Value_ *value, Index_ num_nonzero, Index_ num_all)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>tatami_stats::sum</name>
    <filename>namespacetatami__stats_1_1sum.html</filename>
    <class kind="struct">tatami_stats::sum::RunningDense</class>
    <class kind="struct">tatami_stats::sum::RunningSparse</class>
    <member kind="function">
      <type>Output_</type>
      <name>compute</name>
      <anchorfile>namespacetatami__stats_1_1sum.html</anchorfile>
      <anchor>a3ff9250c76f369ed345487d4a8291959</anchor>
      <arglist>(const Value_ *ptr, Index_ num)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Matrix statistics for tatami</title>
    <filename>index.html</filename>
    <docanchor file="index.html" title="Matrix statistics for tatami">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
