module;
module leprac.literal;
import leprac.config;

namespace leprac {
void Literal::syncLang() { lang_ = *Config::lang(); }
}  // namespace leprac
