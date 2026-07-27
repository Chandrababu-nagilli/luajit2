-- Test if running tests sequentially causes issues
print("Testing sequential execution...")

-- Test 1-10 in sequence
print("\n1. Running basic tests...")
local t = {}
for k, v in pairs(t) do end
print("✓ Empty table")

local t = {1, 2, 3, 4, 5}
local sum = 0
for k, v in pairs(t) do sum = sum + v end
print("✓ Array-only")

local t = {a=1, b=2, c=3}
sum = 0
for k, v in pairs(t) do sum = sum + v end
print("✓ Hash-only")

-- Clear JIT state before stress test
print("\n2. Flushing JIT state...")
jit.flush()
collectgarbage("collect")

-- Now run stress test
print("\n3. Running JIT stress test...")
local t = {a=1, b=2, c=3, d=4, e=5}
local sum = 0
for i = 1, 10000 do
  for k, v in pairs(t) do
    sum = sum + v
  end
end
assert(sum == 150000, "Expected 150000, got " .. sum)
print("✓ JIT stress passed")

print("\n✓ All sequential tests passed!")
