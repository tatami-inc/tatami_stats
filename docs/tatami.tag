<?xml version='1.0' encoding='UTF-8' standalone='yes' ?>
<tagfile doxygen_version="1.9.8">
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
      <anchor>a25a3f87da56ab90796f207b37508f8e7</anchor>
      <arglist>(size_t thread, Index_ start, Index_ length, Output_ *output, Output_ fill)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffer</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffer.html</anchorfile>
      <anchor>af1f9f07457ec2d43b0b6debd2657814d</anchor>
      <arglist>(size_t thread, Index_ start, Index_ length, Output_ *output)</arglist>
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
      <anchor>afaad1036df6e6c82485f1375377973b1</anchor>
      <arglist>(size_t thread, size_t number, Index_ start, Index_ length, GetOutput_ outfun, Output_ fill)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffers</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>a8298dcaaa57f7db497b1476d5a644557</anchor>
      <arglist>(size_t thread, size_t number, Index_ start, Index_ length, GetOutput_ outfun)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LocalOutputBuffers</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>af4c4db4f4578f881b95b7482b4d54ca1</anchor>
      <arglist>()=default</arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>size</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>aad16d06a5c102f48ce95697c5056c402</anchor>
      <arglist>() const</arglist>
    </member>
    <member kind="function">
      <type>Output_ *</type>
      <name>data</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>afade6cb0c028336df221c7d624782a1f</anchor>
      <arglist>(size_t i)</arglist>
    </member>
    <member kind="function">
      <type>const Output_ *</type>
      <name>data</name>
      <anchorfile>classtatami__stats_1_1LocalOutputBuffers.html</anchorfile>
      <anchor>a64ad2eb5863d63ebbe8dbcdf94fdf700</anchor>
      <arglist>(size_t i) const</arglist>
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
    <templarg>bool minimum_</templarg>
    <templarg>typename Output_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningDense</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningDense.html</anchorfile>
      <anchor>a8a7b0887feb925b9e5d47ce372f9aa29</anchor>
      <arglist>(Index_ num, Output_ *store, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningDense.html</anchorfile>
      <anchor>af6aa418d2d28dec63a58109c5d879aa0</anchor>
      <arglist>(const Value_ *ptr)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningDense.html</anchorfile>
      <anchor>a3c6298041d7f986ab68a645ad2c5e636</anchor>
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
    <templarg>bool minimum_</templarg>
    <templarg>typename Output_</templarg>
    <templarg>typename Value_</templarg>
    <templarg>typename Index_</templarg>
    <member kind="function">
      <type></type>
      <name>RunningSparse</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningSparse.html</anchorfile>
      <anchor>a410d814eb15f0488357a57831e64321e</anchor>
      <arglist>(Index_ num, Output_ *store, bool skip_nan, Index_ subtract=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningSparse.html</anchorfile>
      <anchor>aab1432179521651b5e238b04f70ba522</anchor>
      <arglist>(const Value_ *value, const Index_ *index, Index_ number)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>finish</name>
      <anchorfile>classtatami__stats_1_1ranges_1_1RunningSparse.html</anchorfile>
      <anchor>a37f92e38af89b69bd0b6b6afb21ba5cc</anchor>
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
      <type>size_t</type>
      <name>total_groups</name>
      <anchorfile>namespacetatami__stats.html</anchorfile>
      <anchor>ac2384fe2de19a933c735ce79f9bc1b89</anchor>
      <arglist>(const Group_ *group, size_t n)</arglist>
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
      <anchor>aca723eda7d77e04d8b38f215fb1b8f95</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; *p, Output_ *output, int num_threads, Condition_ condition)</arglist>
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
      <anchor>a0f4804cd4dcf6c4017166da931c95c6c</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; *p, Output_ *output, const Options &amp;nopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1nan.html</anchorfile>
      <anchor>a0612f1a96dc5f756e61548e3d645ec6b</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;nopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1nan.html</anchorfile>
      <anchor>a48696e71c3b734a1b8ef345b1f8f9280</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1nan.html</anchorfile>
      <anchor>a6434cfede363ad0331f0817580cad68b</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;nopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1nan.html</anchorfile>
      <anchor>a75cc1885fb2f710fca1b9b5b43224df4</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
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
      <anchor>ab7cd26f29bd8108071cdab2c083f04e7</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; *p, Output_ *output, const Options &amp;zopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1zero.html</anchorfile>
      <anchor>a924cff21ad46e13e0234e72a6bf8fd87</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;zopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1zero.html</anchorfile>
      <anchor>ab9e412c39e073c5665d837e20a262266</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1zero.html</anchorfile>
      <anchor>a9f9260321d40afa4e7ed50e03298b67a</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;zopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1counts_1_1zero.html</anchorfile>
      <anchor>ad453ad64fa06e802b485accd3607ae40</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
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
      <anchor>abbea18f8a5b281048e3eaa11104652d8</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group, const GroupSizes_ &amp;group_sizes, Output_ **output, const Options &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1grouped__medians.html</anchorfile>
      <anchor>a0ec325f6073bf0138c96383e5be42459</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group, const Options &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1grouped__medians.html</anchorfile>
      <anchor>a1b4f02c4e1cc24b9d14c5e9ebdcc9656</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1grouped__medians.html</anchorfile>
      <anchor>a6a1439fbb843b105816e5e6056193399</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group, const Options &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1grouped__medians.html</anchorfile>
      <anchor>a1afc3821f1d0343b8f37c09e8af3bfc4</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group)</arglist>
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
      <anchor>a0dbe7d8241e5f3ea3c2403d85619aa01</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group, size_t num_groups, Output_ **output, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1grouped__sums.html</anchorfile>
      <anchor>a8cefe25992ba2fff2e0ed046ecb299a5</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1grouped__sums.html</anchorfile>
      <anchor>ae8886258e581bd0302cc680cf61897bc</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1grouped__sums.html</anchorfile>
      <anchor>ad2a5614047a31b910c9781838a6e168f</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1grouped__sums.html</anchorfile>
      <anchor>af749a9ed49b540fb1ea4688b4e80b84c</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group)</arglist>
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
      <anchor>a51cbdb71e6e73b0072854f476e00ad99</anchor>
      <arglist>(const Value_ *ptr, Index_ num, const Group_ *group, size_t num_groups, const Index_ *group_size, Output_ *output_means, Output_ *output_variances, bool skip_nan, Index_ *valid_group_size)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>direct</name>
      <anchorfile>namespacetatami__stats_1_1grouped__variances.html</anchorfile>
      <anchor>a25ec363f447893672cc1794d5a92fb3b</anchor>
      <arglist>(const Value_ *value, const Index_ *index, Index_ num_nonzero, const Group_ *group, size_t num_groups, const Index_ *group_size, Output_ *output_means, Output_ *output_variances, Index_ *output_nonzero, bool skip_nan, Index_ *valid_group_size)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1grouped__variances.html</anchorfile>
      <anchor>afb7e174ad083e8f081e53e5454f02a0b</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group, size_t num_groups, const Index_ *group_size, Output_ **output, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1grouped__variances.html</anchorfile>
      <anchor>a7d36990c883e143bb7b034e45d59eb42</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1grouped__variances.html</anchorfile>
      <anchor>ada707945d46bf7c8e0013df9329dadd4</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1grouped__variances.html</anchorfile>
      <anchor>a8da66e302c25e09114cb3cd4d943c665</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1grouped__variances.html</anchorfile>
      <anchor>a20b60121a4232037c908be8e72f99b2c</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Group_ *group)</arglist>
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
      <anchor>ad8b353fbe53c1168d241ac90d392ab67</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; *p, Output_ *output, const medians::Options &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1medians.html</anchorfile>
      <anchor>afe65c01afef3d0dc617a600b4918ae60</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1medians.html</anchorfile>
      <anchor>a0381078ef476435ed35322b668d35a55</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1medians.html</anchorfile>
      <anchor>aec4803eb6b9ba02620a0dd349ea06119</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;mopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1medians.html</anchorfile>
      <anchor>abfe0becd48434812a3f156af66d1aac1</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
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
      <anchor>a7c7085e3a6cd07db268d44d9a137d6ce</anchor>
      <arglist>(const Value_ *ptr, Index_ num, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>Value_</type>
      <name>direct</name>
      <anchorfile>namespacetatami__stats_1_1ranges.html</anchorfile>
      <anchor>a9de5202984cb0179bc97102397d7d917</anchor>
      <arglist>(const Value_ *value, Index_ num_nonzero, Index_ num_all, bool skip_nan)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>apply</name>
      <anchorfile>namespacetatami__stats_1_1ranges.html</anchorfile>
      <anchor>ae6a6bab18f4f4f19e5f11850ea6691c7</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; *p, Output_ *min_out, Output_ *max_out, const Options &amp;ropt)</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; std::vector&lt; Output_ &gt;, std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1ranges.html</anchorfile>
      <anchor>a9dda92fd59e897fb2d58f8197ff645bc</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;ropt)</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; std::vector&lt; Output_ &gt;, std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1ranges.html</anchorfile>
      <anchor>a29e944a7115f37bef97abca5f9bc5e4a</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; std::vector&lt; Output_ &gt;, std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1ranges.html</anchorfile>
      <anchor>a48407bdcf4a30015948b4c1086520068</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;ropt)</arglist>
    </member>
    <member kind="function">
      <type>std::pair&lt; std::vector&lt; Output_ &gt;, std::vector&lt; Output_ &gt; &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1ranges.html</anchorfile>
      <anchor>ae58dd329f93bd692584357cd6931aca8</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
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
      <anchor>ae21d4cc4dfbc7002e3dd57b1eb6047a6</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; *p, Output_ *output, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1sums.html</anchorfile>
      <anchor>a96e18d6b22634a34776883cb5012b0be</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1sums.html</anchorfile>
      <anchor>a607deb708f293f8c36fd831c8ebafe02</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1sums.html</anchorfile>
      <anchor>aef91d6bb0a5132934e63bb2d4844e78d</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;sopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1sums.html</anchorfile>
      <anchor>a540cc9d66aeab27d3c8bb2640f029dbb</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
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
      <anchor>aa569b5ff798e7b55ca6d930711ae6773</anchor>
      <arglist>(bool row, const tatami::Matrix&lt; Value_, Index_ &gt; *p, Output_ *output, const Options &amp;vopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1variances.html</anchorfile>
      <anchor>ac9ec52e261ebef9d024fa88d9318bb26</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;vopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_column</name>
      <anchorfile>namespacetatami__stats_1_1variances.html</anchorfile>
      <anchor>a3fb6b0971c20d5cf2777a6236f0741ca</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1variances.html</anchorfile>
      <anchor>a0069f1a28f0299ae7ebe390ea4c288b6</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p, const Options &amp;vopt)</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; Output_ &gt;</type>
      <name>by_row</name>
      <anchorfile>namespacetatami__stats_1_1variances.html</anchorfile>
      <anchor>a31e03664de1752997833cf4801b377c7</anchor>
      <arglist>(const tatami::Matrix&lt; Value_, Index_ &gt; *p)</arglist>
    </member>
  </compound>
  <compound kind="page">
    <name>index</name>
    <title>Matrix statistics for tatami</title>
    <filename>index.html</filename>
    <docanchor file="index.html">md__2github_2workspace_2README</docanchor>
  </compound>
</tagfile>
