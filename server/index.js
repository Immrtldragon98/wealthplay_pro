import app from './app.js';
import { q } from './db.js';

const port=process.env.PORT||3001;
try {
  await q('SELECT 1');
  console.log('Neon database connection verified');
  app.listen(port,()=>console.log(`Spare Materials API running on port ${port}`));
} catch (error) {
  console.error('Database startup check failed:', error.message);
  process.exit(1);
}
