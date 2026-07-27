-- Minimal test to isolate JIT issue
print("Testing with JIT optimizations disabled...")

-- Disable loop optimization which might be causing issues
jit.opt.start("-loop")

local t = {a=1, b=2, c=3, d=4, e=5}
local sum = 0

print("Running 10000 iterations with -loop...")
for i = 1, 10000 do
  for k, v in pairs(t) do
    sum = sum + v
  end
end

print("✓ Success with -loop: sum=" .. sum)

-- Now try with default optimizations
print("\nRe-enabling all optimizations...")
jit.opt.start(3)  -- Default optimization level

sum = 0
print("Running 10000 iterations with default opts...")
for i = 1, 10000 do
  for k, v in pairs(t) do
    sum = sum + v
  end
end

print("✓ Success with default: sum=" .. sum)
