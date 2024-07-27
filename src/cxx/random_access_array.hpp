#ifndef RANDOM_ACCESS_ARRAY
#define RANDOM_ACCESS_ARRAY
#include <cstdint>
#include <cstdlib>
#include <random>
#include <cassert>

template<typename T>
class RandomAccessArray {
  public:
    RandomAccessArray() : size(0), capacity(0), items(nullptr) {}

    ~RandomAccessArray() {
      if (this->items) 
        free(this->items);
    }

    RandomAccessArray(const RandomAccessArray& other) = delete;
    RandomAccessArray& operator=(const RandomAccessArray& other) = delete;

    RandomAccessArray(RandomAccessArray&& other) noexcept : size(other.size), capacity(other.capacity), items(other.items) {
      other.size = 0;
      other.capacity = 0;
      other.items = nullptr;
    }

    RandomAccessArray& operator=(RandomAccessArray&& other) noexcept {
      if (this != &other) {
        if (this->items) 
          free(this->items);

        this->size = other.size;
        this->capacity = other.capacity;
        this->items = other.items;

        other.size = 0;
        other.capacity = 0;
        other.items = nullptr;
      }
      return *this;
    }

    void Add(T item) {
      if (this->size >= this->capacity) {
        auto new_capacity = 2 * this->capacity + 1;
        if (new_capacity > 32) 
          new_capacity = 32;

        this->items = static_cast<T*>(realloc(this->items, new_capacity * sizeof(T)));
        this->capacity = new_capacity;
      }

      this->items[this->size++] = item;
    }

    T GetRandomItemAndRemove() {
      assert(this->size > 0);

      static std::random_device rd;
      static std::mt19937 gen(rd());
      std::uniform_int_distribution<> dis(0, this->size - 1);

      int i = dis(gen);
      T item = this->items[i];
      this->items[i] = this->items[size - 1];
      this->size--;

      if (this->size == 0) {
        free(this->items);
        this->items = nullptr;
        this->capacity = 0;
      }

      return item;
    }

    inline void Clear() {
      this->size = 0;
    }

    inline uint8_t Size() const {
      return this->size;
    }

  private:
    uint8_t size;
    uint8_t capacity;
    T *items;
};
#endif // RANDOM_ACCESS_ARR
