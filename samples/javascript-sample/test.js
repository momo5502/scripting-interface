const os = require("os");

const mod = { exports: {} };
process.dlopen(mod, process.argv[2], os.constants.RTLD_NOW);

console.log(mod);

mod.exports.hello_world();

const list = ["Hello", "JavaScript"];
mod.exports.print_string_array(list);

mod.exports.run_js_function(arg => {
    console.log(`C++ prints: ${arg}`);
});
