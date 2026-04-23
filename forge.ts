import { createRequire } from 'module';
import { fileURLToPath } from 'url';
import path from 'path';

const require = createRequire(import.meta.url);
const __dirname = path.dirname(fileURLToPath(import.meta.url));

//
// Arg 0 (string)  : VERSION
// Arg 1 (string)  : Path
// Arg 2 (Array)   : IRIDIUM code
// Arg 3 (Array)   : IRIDIUM build context
//
interface IridiumForge {
  execute(
    version: string,
    path: string,
    code: Array<any>,
    buildContext: Array<any>,
    tick: (arg0: string) => void,
    tock: (arg0: string) => void,
    returnLegacyJSONResult: boolean,
  ): Array<any>;
}

/**
 * Your package.json scripts now generate 'forge.cjs' in the project root.
 * This file dynamically points to either the Release or Debug build
 * depending on which script was last run.
 */
const binaryWrapperPath = path.resolve(__dirname, "./forge.cjs");

const forge = require(binaryWrapperPath) as IridiumForge;

export const { execute } = forge;
export default forge;
