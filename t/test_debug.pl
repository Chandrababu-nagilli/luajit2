use lib '.';
use t::TestLJ;
use Test::Base;

my $block = (blocks())[2];  # TEST 3
print "Block name: " . $block->name . "\n";
print "Has jv: " . (defined $block->jv ? "YES" : "NO") . "\n";
print "jv value: '" . ($block->jv // 'undef') . "'\n";
