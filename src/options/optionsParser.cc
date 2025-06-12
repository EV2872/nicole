#include "../../inc/options/optionsParser.h"
#include "version.h"
#include <filesystem>
#include <llvm/Support/CommandLine.h>

namespace nicole {

static llvm::cl::OptionCategory NicoleCategory{"Opciones de nicole.sh"};

static llvm::cl::opt<bool>
    ValidateFlag("s", llvm::cl::desc("Validates AST hierachy (strict mode)"),
                 llvm::cl::ValueDisallowed, llvm::cl::cat(NicoleCategory));

static llvm::cl::alias
    ValidateLongAlias("strict", llvm::cl::aliasopt(ValidateFlag),
                      llvm::cl::desc("Alias for -s (strict mode)"),
                      llvm::cl::cat(NicoleCategory));

static llvm::cl::opt<bool> OptimizeFlag("o", llvm::cl::desc("Optimizer"),
                                        llvm::cl::ValueDisallowed,
                                        llvm::cl::cat(NicoleCategory));

static llvm::cl::alias
    OptimizeLongAlias("optimize", llvm::cl::aliasopt(OptimizeFlag),
                      llvm::cl::desc("Alias for -o (optimizer)"),
                      llvm::cl::cat(NicoleCategory));

static llvm::cl::opt<std::string>
    OutputNameFlag("n", llvm::cl::desc("Executable name (default: a.out)"),
                   llvm::cl::value_desc("file"), llvm::cl::init("a.out"),
                   llvm::cl::cat(NicoleCategory));

static llvm::cl::alias
    OutputNameLongAlias("name", llvm::cl::aliasopt(OptimizeFlag),
                        llvm::cl::desc("Alias for -n (executable name)"),
                        llvm::cl::cat(NicoleCategory));

static llvm::cl::opt<bool> PrintTreeFlag("p", llvm::cl::desc("print AST"),
                                         llvm::cl::ValueDisallowed,
                                         llvm::cl::cat(NicoleCategory));

static llvm::cl::alias
    PrintTreeLongAlias("printTree", llvm::cl::aliasopt(PrintTreeFlag),
                       llvm::cl::desc("Alias for -p (print AST)"),
                       llvm::cl::cat(NicoleCategory));

static llvm::cl::opt<bool> PrintIRFlag("i", llvm::cl::desc("print IR"),
                                       llvm::cl::ValueDisallowed,
                                       llvm::cl::cat(NicoleCategory));

static llvm::cl::alias
    PrintIRLongAlias("printIR", llvm::cl::aliasopt(PrintIRFlag),
                     llvm::cl::desc("Alias for -i (print IR)"),
                     llvm::cl::cat(NicoleCategory));

static llvm::cl::opt<bool> JustInTimeFlag("j", llvm::cl::desc("Mode JIT"),
                                          llvm::cl::ValueDisallowed,
                                          llvm::cl::cat(NicoleCategory));

static llvm::cl::alias JitLongAlias("jit", llvm::cl::aliasopt(JustInTimeFlag),
                                    llvm::cl::desc("Alias for -j (Mode JIT)"),
                                    llvm::cl::cat(NicoleCategory));

static llvm::cl::opt<bool> UseLLDFlag("use-lld",
                                      llvm::cl::desc("Link with LLVM linker"),
                                      llvm::cl::ValueDisallowed,
                                      llvm::cl::cat(NicoleCategory));

// Flag para la arquitectura
static llvm::cl::opt<Arch> ArchFlag(
    "arch", llvm::cl::desc("Target architecture"),
    llvm::cl::values(clEnumValN(Arch::x86, "x86", "Intel 32-bit"),
                     clEnumValN(Arch::x86_64, "x86_64", "Intel 64-bit"),
                     clEnumValN(Arch::arm, "arm", "ARM 32-bit"),
                     clEnumValN(Arch::aarch64, "aarch64", "ARM 64-bit")),
    llvm::cl::init(Arch::x86_64), llvm::cl::cat(NicoleCategory));

// Flag para el sistema operativo
static llvm::cl::opt<OS>
    OSFlag("os", llvm::cl::desc("Operating system target"),
           llvm::cl::values(clEnumValN(OS::Linux, "linux", "Linux"),
                            clEnumValN(OS::MacOS, "macos", "macOS"),
                            clEnumValN(OS::Windows, "windows", "Windows")),
           llvm::cl::init(OS::Linux), llvm::cl::cat(NicoleCategory));

// Flag posicional para el archivo de entrada
static llvm::cl::list<std::string>
    EntryFiles(llvm::cl::Positional,
               llvm::cl::OneOrMore, // permite ≥1 rutas
               llvm::cl::desc("<file.nc>"), llvm::cl::value_desc("path"),
               llvm::cl::cat(NicoleCategory));

auto OptionsParser::parse(int argc, char **argv) noexcept
    -> std::expected<Options, Error> {
  llvm::cl::HideUnrelatedOptions(NicoleCategory);

  llvm::cl::SetVersionPrinter([](llvm::raw_ostream &os) {
    os << "Nicole Compiler versión " << NICOLE_VERSION << "\n";
    os << "Developed by " << NICOLE_AUTHOR << "\n";
    os << "Based on LLVM " << LLVM_VERSION_STRING << "\n";
  });

  llvm::cl::ParseCommandLineOptions(argc, argv, "Nicole compiler\n");

  if (EntryFiles.size() > 1) {
    return createError(ERROR_TYPE::ENTRY_FILE_NOT_FOUND,
                       "by the moment only one entry file is allowed");
  }

  Options opts{OptimizeFlag,   PrintTreeFlag,
               PrintIRFlag,    ValidateFlag,
               JustInTimeFlag, UseLLDFlag,
               OutputNameFlag, std::filesystem::path{EntryFiles.front()},
               ArchFlag,       OSFlag};

  return opts;
}

} // namespace nicole
