# cmake/FindCommonSourceFiles.cmake
# —————————————————————————————————————————————————————————————
# Ficheros comunes de los tests y el binario principal
# —————————————————————————————————————————————————————————————

set(COMMON_SOURCES
    src/options/optionsParser.cc
    src/lexicalAnalysis/type.cc
    src/lexicalAnalysis/tokenStream.cc
    src/lexicalAnalysis/lexer.cc
    src/lexicalAnalysis/nicoleSintax.cc
    src/parsingAnalysis/ast/astType.cc
    src/parsingAnalysis/builder.cc
    src/parsingAnalysis/checkPosition.cc
    src/parsingAnalysis/algorithm/topDown.cc
    src/parsingAnalysis/algorithm/productions/parseBinary.cc
    src/parsingAnalysis/algorithm/productions/parseConditionals.cc
    src/parsingAnalysis/algorithm/productions/parseDecl.cc
    src/parsingAnalysis/algorithm/productions/parseFactor.cc
    src/parsingAnalysis/algorithm/productions/parseFunctions.cc
    src/parsingAnalysis/algorithm/productions/parseLoops.cc
    src/parsingAnalysis/algorithm/productions/parseSelfAssignment.cc
    src/parsingAnalysis/algorithm/productions/parseUserTypes.cc
    src/parsingAnalysis/algorithm/productions/parseUtils.cc
    src/parsingAnalysis/algorithm/productions/parseChained.cc
    src/parsingAnalysis/algorithm/productions/parseTypes.cc

    src/tables/functionTable/functionTable.cc
    src/tables/typeTable/typeTable.cc
    src/tables/typeTable/typeManager.cc
    src/tables/typeTable/userType.cc
    src/tables/typeTable/methodTable.cc
    src/tables/typeTable/attrTable.cc
    src/tables/scope/scope.cc

    # visitors
    src/visitors/printTree/printTree.cc
    src/visitors/printTree/printTreeAssignment.cc
    src/visitors/printTree/printTreeChained.cc
    src/visitors/printTree/printTreeConditionals.cc
    src/visitors/printTree/printTreeEnum.cc
    src/visitors/printTree/printTreeFactor.cc
    src/visitors/printTree/printTreeFunctions.cc
    src/visitors/printTree/printTreeLoops.cc
    src/visitors/printTree/printTreeOperations.cc
    src/visitors/printTree/printTreePointers.cc
    src/visitors/printTree/printTreeUserTypes.cc
    src/visitors/printTree/printTreeUtils.cc
    src/visitors/printTree/printTreeVarDecl.cc
    src/visitors/printTree/printTreeVector.cc

    ######
    src/visitors/validateTree/validateTree.cc
    src/visitors/validateTree/validateAssignment.cc
    src/visitors/validateTree/validateChained.cc
    src/visitors/validateTree/validateConditionals.cc
    src/visitors/validateTree/validateEnum.cc
    src/visitors/validateTree/validateFactor.cc
    src/visitors/validateTree/validateFunctions.cc
    src/visitors/validateTree/validateLoops.cc
    src/visitors/validateTree/validateOperations.cc
    src/visitors/validateTree/validatePointers.cc
    src/visitors/validateTree/validateUserTypes.cc
    src/visitors/validateTree/validateUtils.cc
    src/visitors/validateTree/validateVarDecl.cc
    src/visitors/validateTree/validateVector.cc
    
    #####
    src/visitors/fillSemanticInfo/fillSemanticInfo.cc
    src/visitors/fillSemanticInfo/fillSemanticAssignment.cc
    src/visitors/fillSemanticInfo/fillSemanticChained.cc
    src/visitors/fillSemanticInfo/fillSemanticConditionals.cc
    src/visitors/fillSemanticInfo/fillSemanticEnum.cc
    src/visitors/fillSemanticInfo/fillSemanticFactor.cc
    src/visitors/fillSemanticInfo/fillSemanticFunctions.cc
    src/visitors/fillSemanticInfo/fillSemanticLoops.cc
    src/visitors/fillSemanticInfo/fillSemanticOperations.cc
    src/visitors/fillSemanticInfo/fillSemanticPointers.cc
    src/visitors/fillSemanticInfo/fillSemanticUserTypes.cc
    src/visitors/fillSemanticInfo/fillSemanticUtils.cc
    src/visitors/fillSemanticInfo/fillSemanticVarDecl.cc
    src/visitors/fillSemanticInfo/fillSemanticVector.cc

    #####
    src/visitors/monomorphize/monomorphize.cc
    src/visitors/monomorphize/monomorphizeAssignment.cc
    src/visitors/monomorphize/monomorphizeChained.cc
    src/visitors/monomorphize/monomorphizeConditionals.cc
    src/visitors/monomorphize/monomorphizeEnum.cc
    src/visitors/monomorphize/monomorphizeFactor.cc
    src/visitors/monomorphize/monomorphizeFunctions.cc
    src/visitors/monomorphize/monomorphizeLoops.cc
    src/visitors/monomorphize/monomorphizeOperations.cc
    src/visitors/monomorphize/monomorphizePointers.cc
    src/visitors/monomorphize/monomorphizeUserTypes.cc
    src/visitors/monomorphize/monomorphizeUtils.cc
    src/visitors/monomorphize/monomorphizeVarDecl.cc
    src/visitors/monomorphize/monomorphizeVector.cc
    
    #####
    src/visitors/typeAnalysis/typeAnalysis.cc
    src/visitors/typeAnalysis/typeAnalysisAssignment.cc
    src/visitors/typeAnalysis/typeAnalysisChained.cc
    src/visitors/typeAnalysis/typeAnalysisConditionals.cc
    src/visitors/typeAnalysis/typeAnalysisEnum.cc
    src/visitors/typeAnalysis/typeAnalysisFactor.cc
    src/visitors/typeAnalysis/typeAnalysisFunctions.cc
    src/visitors/typeAnalysis/typeAnalysisLoops.cc
    src/visitors/typeAnalysis/typeAnalysisOperations.cc
    src/visitors/typeAnalysis/typeAnalysisPointers.cc
    src/visitors/typeAnalysis/typeAnalysisUserTypes.cc
    src/visitors/typeAnalysis/typeAnalysisUtils.cc
    src/visitors/typeAnalysis/typeAnalysisVarDecl.cc
    src/visitors/typeAnalysis/typeAnalysisVector.cc
    
    #####
    src/visitors/codeGeneration/codeGeneration.cc
    src/visitors/codeGeneration/codeGenAssignment.cc
    src/visitors/codeGeneration/codeGenChained.cc
    src/visitors/codeGeneration/codeGenConditionals.cc
    src/visitors/codeGeneration/codeGenEnum.cc
    src/visitors/codeGeneration/codeGenFactor.cc
    src/visitors/codeGeneration/codeGenFunctions.cc
    src/visitors/codeGeneration/codeGenLoops.cc
    src/visitors/codeGeneration/codeGenOperations.cc
    src/visitors/codeGeneration/codeGenPointers.cc
    src/visitors/codeGeneration/codeGenUserTypes.cc
    src/visitors/codeGeneration/codeGenUtils.cc
    src/visitors/codeGeneration/codeGenVarDecl.cc
    src/visitors/codeGeneration/codeGenVector.cc

    src/compiler/nicole.cc
    src/compiler/optimizer.cc
    src/compiler/jitEngine.cc
    src/compiler/linker.cc
    src/compiler/targetGenerator.cc

    # utils
    src/utils/nameMangler.cc
)