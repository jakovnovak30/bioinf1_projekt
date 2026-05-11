template <typename T>
class HashFunction {
public:
  HashFunction() = default;

  virtual T hash(int val) = 0;
  virtual size_t max_res() = 0;
};
