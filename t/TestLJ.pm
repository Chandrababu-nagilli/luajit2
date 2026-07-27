package t::TestLJ;

use v5.10.1;
use Test::Base -Base;
use IPC::Open3;
use Cwd qw( cwd );
use Test::LongString;
use File::Temp qw( tempdir );
use Symbol qw( gensym );

our @EXPORT = qw( run_tests );

my $cwd = cwd;
$ENV{LUA_CPATH} = "$cwd/src/?.so;;";
$ENV{LUA_PATH} = "$cwd/src/?.lua;$cwd/src/?/?.lua;;";
#$ENV{LUA_PATH} = ($ENV{LUA_PATH} || "" ) . ';' . getcwd . "/runtime/?.lua" . ';;';

my $luajit = "$cwd/src/luajit";

sub run_test ($) {
    my $block = shift;
    #print $json_xs->pretty->encode(\@new_rows);
    #my $res = #print $json_xs->pretty->encode($res);
    my $name = $block->name;

    my $lua = $block->lua or
        die "No --- lua specified for test $name\n";

    my $dir = tempdir("testlj_XXXXXXX", DIR => File::Temp::tempdir(CLEANUP => 0), CLEANUP => 1);
    my $luafile = "$dir/test.lua";

    {
        open my $fh, ">$luafile"
            or die "$name - Cannot open $luafile in $dir for writing: $!\n";
        print $fh $lua;
        close $fh;
    }

    my ($res, $err);

    my @cmd;

    if ($ENV{TEST_LJ_USE_VALGRIND}) {
        warn "$name\n";
        @cmd =  ('valgrind', '-q', '--leak-check=full', $luajit,
                 ($block->jv // '') ne '' ? '-jv' : (),
                 ($block->jdump // '') ne '' ? '-jdump' : (),
                 $luafile);
    } else {
        @cmd =  ($luajit,
                 ($block->jv // '') ne '' ? '-jv' : (),
                 ($block->jdump // '') ne '' ? '-jdump' : (),
                 $luafile);
    }

    {
        my ($out, $errfh);
        $errfh = gensym();
        my $pid = open3(undef, $out, $errfh, @cmd);
        local $/;
        $res = <$out>;
        $err = <$errfh>;
        waitpid($pid, 0);
    }
    my $rc = $?;

    if ($ENV{TEST_LJ_DEBUG}) {
        warn "CMD=@cmd\n";
        warn "RC=" . ($rc >> 8) . "\n";
        warn "STDOUT<<$res>>\n";
        warn "STDERR<<$err>>\n";
    }

    my $exp_rc = $block->exit // 0;

    is $exp_rc, $rc >> 8, "$name - exit code okay";

    my $exp_err = $block->err;
    if (defined $exp_err) {
        if ($err =~ /.*:.*:.*: (.*\s)?/) {
            $err = $1;
        }

	if (ref $exp_err) {
	  like $err, $exp_err, "$name - err like expected";

	} else {
	  is $err, $exp_err, "$name - err expected";
	}

    } elsif (defined $err && $err ne '') {
        warn "$name - STDERR:\n$err";
    }

    if (defined $block->out) {
        #is $res, $block->out, "$name - output ok";
        is $res, $block->out, "$name - output ok";

    } elsif (defined $res && $res ne '') {
        warn "$name - STDOUT:\n$res";
    }

    chdir $cwd or die $!;
}

sub run_tests () {
    for my $block (blocks()) {
        run_test($block);
    }
}

1;
