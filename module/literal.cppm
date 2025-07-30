module;
export module leprac.literal;
import leprac.common;

export namespace leprac {
class Literal {
 public:
  static void syncLang();

 private:
  inline static Lang lang_;
};
}  // namespace leprac
