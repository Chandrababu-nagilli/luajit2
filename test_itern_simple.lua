-- Simple BC_ITERN test
jit.off()  -- Disable JIT to test interpreter only

local t = {a=1, b=2, c=3}
local sum = 0
local count = 0

print("Testing iterator with JIT OFF:")
for k, v in pairs(t) do
  print("  key:", k, "value:", v)
  sum = sum + v
  count = count + 1
end

print("Sum:", sum, "Count:", count)
assert(sum == 6, "Expected sum 6, got " .. sum)
assert(count == 3, "Expected count 3, got " .. count)

print("\nNow testing with JIT ON:")
jit.on()

local t2 = {x=10, y=20, z=30}
local sum2 = 0
local count2 = 0

-- Warm up
for i = 1, 100 do
  local s = 0
  for k, v in pairs(t2) do
    s = s + v
  end
end

for k, v in pairs(t2) do
  print("  key:", k, "value:", v)
  sum2 = sum2 + v
  count2 = count2 + 1
end

print("Sum:", sum2, "Count:", count2)
assert(sum2 == 60, "Expected sum 60, got " .. sum2)
assert(count2 == 3, "Expected count 3, got " .. count2)

print("\n✓ All tests passed!")
