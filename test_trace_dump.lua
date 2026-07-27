-- Enable verbose JIT dumping to see trace compilation
jit.opt.start("hotloop=2", "hotexit=1")
jit.dump = require("jit.dump")
jit.dump.on("tbimrs", "trace_dump.txt")

print("=== Trace Dump Test ===\n")

local function run_test(num)
  print(string.format("Test %d: Running pairs() loop...", num))
  local t = {a=1, b=2, c=3, d=4, e=5}
  local sum = 0
  
  for i = 1, 1000 do
    for k, v in pairs(t) do
      sum = sum + v
    end
  end
  
  print(string.format("  Result: %d", sum))
  assert(sum == 15000, "Sum mismatch")
end

-- Run tests until segfault (should happen around test 6)
for i = 1, 10 do
  run_test(i)
  collectgarbage("collect")
end

jit.dump.off()
print("\n✓ All tests completed")
